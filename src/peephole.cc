#include "peephole.hh"
#include "codegen.hh"

namespace codegen {

static const std::vector<std::pair<PixIRPattern, CodePeephole>> patterns{
    {PixIRPattern({{PixIROpcode::PUSH, "0"}, {PixIROpcode::ALLOC}}), {}},
    {PixIRPattern(
         {{PixIROpcode::GT}, {PixIROpcode::PUSH, "1"}, {PixIROpcode::SUB}}),
     {{PixIROpcode::LE}}},
    {PixIRPattern(
         {{PixIROpcode::LT}, {PixIROpcode::PUSH, "1"}, {PixIROpcode::SUB}}),
     {{PixIROpcode::GE}}},
    {PixIRPattern(
         {{PixIROpcode::GE}, {PixIROpcode::PUSH, "1"}, {PixIROpcode::SUB}}),
     {{PixIROpcode::LT}}},
    {PixIRPattern(
         {{PixIROpcode::LE}, {PixIROpcode::PUSH, "1"}, {PixIROpcode::SUB}}),
     {{PixIROpcode::GT}}},
};

void peepholeOptimize(PixIRCode &code) {
  for (std::unique_ptr<PixIRFunction> &func : code) {
    for (std::unique_ptr<BasicBlock> &block : func->blocks) {
      for (auto it = block->instrs.begin(); it != block->instrs.end(); ++it) {
        for (auto const &[pattern, substitute] : patterns) {
          it = pattern.match_and_replace(block->instrs, it, block->instrs.end(),
                                         substitute);
        }
      }
    }
  }
}

} // namespace codegen
