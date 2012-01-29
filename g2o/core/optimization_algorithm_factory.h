// g2o - General Graph Optimization
// Copyright (C) 2011 R. Kuemmerle, G. Grisetti, W. Burgard
// 
// g2o is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// g2o is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef G2O_OPTMIZATION_ALGORITHM_PROPERTY_H
#define G2O_OPTMIZATION_ALGORITHM_PROPERTY_H

#include "optimization_algorithm_property.h"

#include <list>
#include <iostream>
#include <typeinfo>

#include "g2o_core_api.h"

namespace g2o {

  // forward decl
  class G2O_CORE_API OptimizationAlgorithm;
  class G2O_CORE_API SparseOptimizer;

  /**
   * \brief base for allocating an optimization algorithm
   *
   * Allocating a solver for a given optimizer. The method construct() has to be
   * implemented in your derived class to allocate the desired solver.
   */
  class G2O_CORE_API AbstractOptimizationAlgorithmCreator
  {
    public:
      AbstractOptimizationAlgorithmCreator(const OptimizationAlgorithmProperty& p);
      //! allocate a solver operating on optimizer, re-implement for your creator
      virtual OptimizationAlgorithm* construct() = 0;
      //! return the properties of the solver
      const OptimizationAlgorithmProperty& property() const { return _property;}
    protected:
      OptimizationAlgorithmProperty _property;
  };
  
  /**
   * \brief create solvers based on their short name
   *
   * Factory to allocate solvers based on their short name.
   * The Factory is implemented as a sigleton and the single
   * instance can be accessed via the instance() function.
   */
  class G2O_CORE_API OptimizationAlgorithmFactory
  {
    public:
      typedef std::list<AbstractOptimizationAlgorithmCreator*>      CreatorList;

      //! return the instance
      static OptimizationAlgorithmFactory* instance();

      //! free the instance
      static void destroy();

      /**
       * register a specific creator for allocating a solver
       */
      void registerSolver(AbstractOptimizationAlgorithmCreator* c);

      /**
       * unregister a specific creator for allocating a solver
       */
      void unregisterSolver(AbstractOptimizationAlgorithmCreator* c);
      
      /**
       * construct a solver based on its name, e.g., var, fix3_2_cholmod
       */
      OptimizationAlgorithm* construct(const std::string& tag, OptimizationAlgorithmProperty& solverProperty) const;

      //! list the known solvers into a stream
      void listSolvers(std::ostream& os) const;

      //! return the underlying list of creators
      const CreatorList& creatorList() const { return _creator;}

    protected:
      OptimizationAlgorithmFactory();
      ~OptimizationAlgorithmFactory();

      CreatorList _creator;

      CreatorList::const_iterator findSolver(const std::string& name) const;
      CreatorList::iterator findSolver(const std::string& name);

    private:
      static OptimizationAlgorithmFactory* factoryInstance;
  };

  class RegisterOptimizationAlgorithmProxy
    {
      public:
      RegisterOptimizationAlgorithmProxy(AbstractOptimizationAlgorithmCreator* c)
          {
            _creator = c;
#ifndef NDEBUG
            std::cout << __FUNCTION__ << ": Registering " << _creator->property().name << " of type " << typeid(*_creator).name() << std::endl;
#endif
            OptimizationAlgorithmFactory::instance()->registerSolver(c);
          }
      
        ~RegisterOptimizationAlgorithmProxy()
          {
#ifndef NDEBUG
            std::cout << __FUNCTION__ << ": Unregistering " << _creator->property().name << std::endl;
#endif
            OptimizationAlgorithmFactory::instance()->unregisterSolver(_creator);
          }
    private:
        AbstractOptimizationAlgorithmCreator* _creator;
  };

}

#define G2O_REGISTER_OPTIMIZATION_LIBRARY(libraryname) \
    extern "C" void g2o_optimization_library_##libraryname(void) {}

#define G2O_USE_OPTIMIZATION_LIBRARY(libraryname) \
    extern "C" void g2o_optimization_library_##libraryname(void); \
    static g2o::ForceLinker g2o_force_optimization_algorithm_library_##classname(g2o_optimization_library_##libraryname);

#define G2O_REGISTER_OPTIMIZATION_ALGORITHM(optimizername, instance) \
    extern "C" void g2o_optimization_algorithm_##optimizername(void) {} \
    static g2o::RegisterOptimizationAlgorithmProxy g_optimization_algorithm_proxy_##optimizername(instance);

#define G2O_USE_OPTIMIZATION_ALGORITHM(optimizername) \
    extern "C" void g2o_optimization_algorithm_##optimizername(void); \
    static g2o::ForceLinker g2o_force_optimization_algorithm_link_##optimizername(g2o_optimization_algorithm_##optimizername);

#endif
