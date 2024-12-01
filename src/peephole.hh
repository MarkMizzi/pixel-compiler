#ifndef PEEPHOLE_H_
#define PEEPHOLE_H_

#include "codegen.hh"

#include <list>
#include <utility>
#include <variant>

namespace codegen
{

  using CodePeephole = std::list<PixIRInstruction>;

  class PixIRPattern
  {
  private:
    CodePeephole pattern;

  public:
    PixIRPattern(CodePeephole &&pattern) : pattern(std::move(pattern)) {}

    bool match(CodePeephole::iterator codeIt,
               const CodePeephole::iterator codeEnd) const
    {

      auto patternIt = pattern.cbegin();
      for (; codeIt != codeEnd && patternIt != pattern.cend();
           ++codeIt, ++patternIt)
      {
        if (codeIt->opcode != patternIt->opcode)
        {
          return false;
        }
        if (codeIt->opcode == PixIROpcode::PUSH &&
            // std::monostate acts as a wildcard that matches any PUSH operand.
            !std::holds_alternative<std::monostate>(patternIt->data) &&
            codeIt->data != patternIt->data)
        {
          return false;
        }
      }

      if (patternIt != pattern.cend())
      {
        return false;
      }

      return true;
    }

    CodePeephole::iterator
    match_and_replace(CodePeephole &codeRun, CodePeephole::iterator codeIt,
                      const CodePeephole::iterator codeEnd,
                      const CodePeephole &substitute) const
    {

      if (match(codeIt, codeEnd))
      {
        codeIt = codeRun.erase(codeIt, std::next(codeIt, pattern.size()));
        return codeRun.insert(codeIt, substitute.begin(), substitute.end());
      }

      return codeIt;
    }
  };

  void peepholeOptimize(PixIRCode &code);

} // end namespace codegen

#endif // PEEPHOLE_H_
