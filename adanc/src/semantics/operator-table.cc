#include "operator-table.hh"
#include "registration.hh"

OperatorTable::OperatorTable()
{
    register_binary_arithmetic(*this);
    register_binary_assignment(*this);
    register_binary_comparison(*this);

    register_unary(*this);
}