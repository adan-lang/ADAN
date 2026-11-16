#include <llvm/IR/Instructions.h>

#include <format>
#include <vector>

#include "lower.hh"

void IRLowerer::visit(CallExpr &node)
{
    llvm::Function *function = _module->getFunction(node.callee);
    if (function == nullptr)
        diagnostics.error(std::format("No LLVM declaration exists for function: \"{}\"", node.callee));

    if (node.args.size() != function->arg_size())
        diagnostics.error(std::format("Call argument count does not match the function declaration for {}", node.callee));

    std::vector<llvm::Value *> args;
    args.reserve(node.args.size());
    for (std::size_t i{}; i < node.args.size(); ++i)
    {
        llvm::Value *lowered = lower_expr(*node.args[i]);
        if (lowered->getType()->isVoidTy())
            diagnostics.error("A void function call cannot be used as an argument");
        if (lowered->getType() != function->getFunctionType()->getParamType(static_cast<unsigned>(i)))
            diagnostics.error(std::format("Call argument type does not match the function declaration for {}", node.callee));

        args.emplace_back(lowered);
    }

    result = builder.CreateCall(function, args);
}

void IRLowerer::visit(Identifier &node)
{
    const auto value = local_values.find(node.name);
    if (value == local_values.end())
        diagnostics.error("No lowered value exists for identifier");

    auto instance = llvm::dyn_cast<llvm::AllocaInst>(value->second);
    if (instance == nullptr)
        diagnostics.error(std::format("Identifier: \"{}\" does not refer to local storage", node.name));

    llvm::LoadInst *loaded = builder.CreateLoad(instance->getAllocatedType(), value->second);
    result = loaded;
}