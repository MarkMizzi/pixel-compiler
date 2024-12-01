#ifndef DEADCODE_H_
#define DEADCODE_H_

#include "codegen.hh"

#include <set>
#include <string>
#include <unordered_map> // use unordered_* versions of sets/maps for hot vars.
#include <unordered_set>
#include <variant>

namespace codegen
{

  class DeadFunctionEliminator
  {
  private:
    std::vector<PixIRFunction *> workList;
    // mostly for speeding things up.
    std::unordered_map<std::string, PixIRFunction *> funcs;
    PixIRCode &code;

    std::set<std::string> calleesOf(const PixIRFunction &func)
    {
      std::set<std::string> callees;

      std::string funcName;
      for (const std::unique_ptr<BasicBlock> &block : func.blocks)
      {
        for (const PixIRInstruction &instr : block->instrs)
        {
          if (instr.opcode == PixIROpcode::PUSH &&
              std::holds_alternative<std::string>(instr.data) &&
              (funcName = std::get<std::string>(instr.data)).rfind(".", 0) == 0)
          {
            callees.insert(funcName);
          }
        }
      }
      return callees;
    }

  public:
    DeadFunctionEliminator(PixIRCode &code) : code(code), funcs(code.size())
    {
      for (const std::unique_ptr<PixIRFunction> &func : code)
      {
        funcs.insert({func->funcName, func.get()});
      }
    }

    std::unordered_set<std::string> findReachable()
    {
      std::unordered_set<std::string> reachable{std::string(".") +
                                                MAIN_FUNC_NAME};
      workList = {funcs.at(std::string(".") + MAIN_FUNC_NAME)};

      while (workList.size() > 0)
      {
        PixIRFunction *back = *--workList.end();
        workList.pop_back();

        for (const std::string &funcName : calleesOf(*back))
        {
          if (!reachable.count(funcName))
          {
            reachable.insert(funcName);
            workList.push_back(funcs.at(funcName));
          }
        }
      }

      return reachable;
    }

    void eliminate()
    {
      std::unordered_set<std::string> reachable{findReachable()};
      for (auto it = code.begin(); it != code.end(); ++it)
      {
        if (!reachable.count((*it)->funcName))
        {
          --it;
          code.erase(it + 1);
        }
      }
    }
  };

  // another type of dead code elimination
  // Basically if we generate code after a return in a basic block, this is
  // dead code.
  void eliminateDeadCodeAfterReturn(PixIRCode &code);

} // end namespace codegen

#endif // DEADCODE_H_
