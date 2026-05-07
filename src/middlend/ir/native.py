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
    so = os.path.join(BinDir, "ir_core.so")
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
                here,
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
        lib.ir_new.restype = ctypes.c_void_p
        lib.ir_new.argtypes = [ctypes.c_char_p]
        for fn in ("ir_pretty_print", "ir_build_cfg", "ir_liveness"):
            getattr(lib, fn).restype = ctypes.c_void_p
            getattr(lib, fn).argtypes = [ctypes.c_void_p]
        lib.ir_free.restype = lib.ir_free_str.restype = None
        lib.ir_free.argtypes = lib.ir_free_str.argtypes = [ctypes.c_void_p]
        Lib = lib
    except Exception:
        pass
    return Lib


def Call(fn: str, json_ir: str) -> str:
    lib = Load()
    if not lib:
        return json_ir
    h = lib.ir_new(json_ir.encode())
    if not h:
        return json_ir
    ptr = getattr(lib, fn)(h)
    result = ctypes.string_at(ptr).decode() if ptr else ""
    if ptr:
        lib.ir_free_str(ptr)
    lib.ir_free(h)
    return result


def PrettyPrint(json_ir: str) -> str:
    return Call("ir_pretty_print", json_ir)


def BuildCFG(json_ir: str) -> str:
    return Call("ir_build_cfg", json_ir)


def Liveness(json_ir: str) -> str:
    return Call("ir_liveness", json_ir)
