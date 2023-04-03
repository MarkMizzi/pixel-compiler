#include "deadcode.hh"

namespace codegen {
void eliminateDeadCodeAfterReturn(PixIRCode &code) {
  for (std::unique_ptr<PixIRFunction> &func : code) {
    for (std::unique_ptr<BasicBlock> &block : func->blocks) {
      for (auto it = block->instrs.begin(); it != block->instrs.end(); ++it) {
        if (it->opcode == PixIROpcode::RET) {
          block->instrs.erase(++it, block->instrs.end());
          break;
        }
      }
    }
  }
}

} // end namespace codegen
