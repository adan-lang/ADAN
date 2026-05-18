#include "reader.hpp"
#include "emitter.hpp"


IRVal Reader::ReadValue(const IRVal& val, Emitter& emitter) {
    switch (val.kind) {
        case 'C': return ReadConst(val, emitter);
        case 'V': return ReadLoad(val, emitter);
        case 'T': return val;
        default:
            throw std::runtime_error(
                std::string("Unknown IRVal kind: ") + val.kind
            );
    }
}

IRVal Reader::ReadInstr(const IRInstr& instr, Emitter& emitter) {
    if (instr.op == "BINARY") return ReadBinary(instr, emitter);
    if (instr.op == "UNARY")  return ReadUnary(instr, emitter);
    if (instr.op == "CALL")   return ReadCall(instr, emitter);
    if (instr.op == "INTERP") return ReadInterp(instr, emitter);
    throw std::runtime_error("Unhandled opcode in Reader: " + instr.op);
}

std::vector<IRVal> Reader::ReadValues(
    const std::vector<IRVal>& vals,
    Emitter& emitter
) {
    std::vector<IRVal> results;
    results.reserve(vals.size());
    for (const auto& v : vals)
        results.push_back(ReadValue(v, emitter));
    return results;
}

std::string Reader::InferLiteralType(const IRVal& val) {
    if (val.is_bool) return "BOOL";
    if (val.is_null) return "NONE";
    if (val.is_str)  return "STRING";
    return "NUMBER";
}

IRVal Reader::ReadConst(const IRVal& val, Emitter& emitter) {
    if (val.kind != 'C') {
        throw std::runtime_error(
            "ReadConst called with non-const IRVal (kind: " + std::string(1, val.kind) + ")"
        );
    }

    IRVal c = val;
    if (c.type.empty() || c.type == "UNKNOWN") {
        c.type = InferLiteralType(c);
    }
    return emitter.EmitConst(c);
}

IRVal Reader::ReadLoad(const IRVal& val, Emitter& emitter) {
    // TODO: Emit a LOAD instruction for the named variable (val.name)
    return val;
}

IRVal Reader::ReadBinary(const IRInstr& instr, Emitter& emitter) {
    // TODO: Read left/right operands then emit a BINARY instruction
    // const IRVal& left  = ReadValue(instr.operands[0], emitter);
    // const IRVal& right = ReadValue(instr.operands[1], emitter);
    // return emitter.EmitBinary(instr.extra_str, left, right);
    return instr.dst;
}

IRVal Reader::ReadUnary(const IRInstr& instr, Emitter& emitter) {
    // TODO: Read operand then emit a UNARY instruction
    // const IRVal& operand = ReadValue(instr.operands[0], emitter);
    // return emitter.EmitUnary(instr.extra_str, operand);
    return instr.dst;
}

IRVal Reader::ReadCall(const IRInstr& instr, Emitter& emitter) {
    // TODO: Read args then emit a CALL instruction
    // std::vector<IRVal> args = ReadValues(instr.operands, emitter);
    // return emitter.EmitCall(instr.extra_str, args);
    return instr.dst;
}

IRVal Reader::ReadInterp(const IRInstr& instr, Emitter& emitter) {
    // TODO: Read parts then emit an INTERP instruction
    // std::vector<IRVal> parts = ReadValues(instr.operands, emitter);
    // return emitter.EmitInterp(parts);
    return instr.dst;
}
