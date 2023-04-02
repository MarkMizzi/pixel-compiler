#ifndef PEEPHOLE_H_
#define PEEPHOLE_H_

#include "codegen.hh"

#include <list>
#include <utility>
#include <variant>

namespace codegen {

using CodePeephole = std::list<PixIRInstruction>;

class PixIRPattern {
private:
  CodePeephole pattern;

public:
  PixIRPattern(CodePeephole &&pattern) : pattern(pattern) {}

  bool match(CodePeephole::iterator codeIt,
             const CodePeephole::iterator code_end) const {

    auto patternIt = pattern.cbegin();
    for (; codeIt != code_end && patternIt != pattern.cend();
         ++codeIt, ++patternIt) {
      if (codeIt->opcode != patternIt->opcode) {
        return false;
      }
      if (codeIt->opcode == PixIROpcode::PUSH &&
          !std::holds_alternative<std::monostate>(patternIt->data) &&
          codeIt->data != patternIt->data) {
        return false;
      }
    }
    if (patternIt != pattern.cend()) {
      return false;
    }
    return true;
  }

  CodePeephole::iterator
  match_and_replace(CodePeephole &codeRun, CodePeephole::iterator codeIt,
                    const CodePeephole::iterator codeEnd,
                    const CodePeephole &substitute) const {

    CodePeephole substituteCopy = substitute;
    if (match(codeIt, codeEnd)) {
      codeIt = codeRun.erase(codeIt, std::next(codeIt, pattern.size()));
      return codeRun.insert(codeIt, substitute.begin(), substitute.end());
    }
    return codeIt;
  }
};

void peepholeOptimize(PixIRCode &code);

} // end namespace codegen

#endif // PEEPHOLE_H_
