import ctypes
import os
import subprocess

Lib = None
Done = False
BinDir = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "bin"
)


def Load():
    global Lib, Done
    if Done:
        return Lib
    Done = True
    here = os.path.dirname(os.path.abspath(__file__))
    so = os.path.join(BinDir, "opt_core.so")
    os.makedirs(BinDir, exist_ok=True)
    if not os.path.exists(so):
        r = subprocess.run(
            [
                "g++",
                "-O2",
                "-std=c++17",
                "-shared",
                "-fPIC",
                "-I",
                os.path.join(here, "..", "ir"),
                "-o",
                so,
                os.path.join(here, "core.cpp"),
            ],
            capture_output=True,
        )
        if r.returncode != 0:
            return None
    try:
        lib = ctypes.CDLL(so)
        for fn in ("opt_fold_constants", "opt_dead_code_elim", "opt_cse", "opt_all"):
            getattr(lib, fn).restype = ctypes.c_void_p
        lib.opt_fold_constants.argtypes = lib.opt_dead_code_elim.argtypes = (
            lib.opt_cse.argtypes
        ) = [ctypes.c_char_p]
        lib.opt_all.argtypes = [ctypes.c_char_p, ctypes.c_int]
        lib.opt_free_str.restype = None
        lib.opt_free_str.argtypes = [ctypes.c_void_p]
        Lib = lib
    except Exception:
        pass
    return Lib


def Run(fn: str, json_ir: str, *extra) -> str:
    lib = Load()
    if not lib:
        return json_ir
    ptr = getattr(lib, fn)(json_ir.encode(), *extra)
    if not ptr:
        return json_ir
    result = ctypes.string_at(ptr).decode()
    lib.opt_free_str(ptr)
    return result


def FoldConstants(json_ir: str) -> str:
    return Run("opt_fold_constants", json_ir)


def DeadCodeElim(json_ir: str) -> str:
    return Run("opt_dead_code_elim", json_ir)


def CSE(json_ir: str) -> str:
    return Run("opt_cse", json_ir)


def RunAll(json_ir: str, passes: int = 3) -> str:
    return Run("opt_all", json_ir, ctypes.c_int(passes))
