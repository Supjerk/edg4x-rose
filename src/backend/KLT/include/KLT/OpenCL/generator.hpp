
#ifndef __KLT_OPENCL_GENERATOR_HPP__
#define __KLT_OPENCL_GENERATOR_HPP__

#include "KLT/Core/generator.hpp"

namespace KLT {

namespace OpenCL {

class Generator : public virtual Core::Generator {
  public:
    Generator();
    virtual ~Generator();

    virtual bool generate(const Core::LoopTrees & loop_trees, std::set<Core::Kernel *> & kernels, std::map<unsigned long, std::set<unsigned long> > & kernel_deps);
};

}

}

#endif /* __KLT_OPENCL_GENERATOR_HPP__ */
