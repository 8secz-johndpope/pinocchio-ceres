//
// Copyright (c) 2018 INRIA
//
// This file is part of Pinocchio-Ceres
// pinocchio is free software: you can redistribute it
// and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// pinocchio is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Lesser Public License for more details. You should have
// received a copy of the GNU Lesser General Public License along with
// pinocchio If not, see
// <http://www.gnu.org/licenses/>.
//

#ifndef __pinocchio_ceres_shift_residual_hpp__
#define __pinocchio_ceres_shift_residual_hpp__

#include <ceres/ceres.h>
#include <Eigen/Core>

namespace pinocchio
{
  namespace ceres
  {
    ///
    /// \brief Helper to shift a resiudal from a given vector value called shift_value.
    ///
    struct ShiftResidual : ::ceres::CostFunction
    {
      
      typedef Eigen::VectorXd VectorType;
      
      /// \brief Default constructor.
      ShiftResidual(::ceres::CostFunction * cost)
      : cost(cost)
      {
        assert(cost != NULL && "The cost is NULL");
        *mutable_parameter_block_sizes() = cost->parameter_block_sizes();
        set_num_residuals(cost->num_residuals());
        shift_value = Eigen::VectorXd::Zero(cost->num_residuals());
      }
      
      /// \brief Default constructor with a given shift vector.
      template<typename VectorLike>
      ShiftResidual(::ceres::CostFunction * cost,
                    const Eigen::MatrixBase<VectorLike> & shift)
      : cost(cost)
      , shift_value(shift)
      {
        assert(cost != NULL && "The cost is NULL");
        assert(shift_value.size() == cost->num_residuals());
        *mutable_parameter_block_sizes() = cost->parameter_block_sizes();
        set_num_residuals(cost->num_residuals());
      }
      
      bool Evaluate(double const* const* x,
                    double* residuals,
                    double** jacobians) const
      {
        assert(x != NULL && "x is NULL");
        
        const bool res = cost->Evaluate(x,residuals,jacobians);
        
        if(residuals != NULL)
        {
          Eigen::Map<VectorType> residuals_map(residuals,shift_value.size(),1);
          residuals_map += shift_value;
        }
        
        return res;
      }
      
      template<typename VectorLike>
      void setShiftValue(const Eigen::MatrixBase<VectorLike> & shift_value_new)
      { shift_value = shift_value_new; }
      const VectorType & getShiftValue() const { return shift_value; }
      
      const ::ceres::CostFunction * getCost() const { return cost; }
      
    protected:

      /// \brief The cost residual to shift
      ::ceres::CostFunction * cost;
      
      /// \brief The shiffting vector value.
      VectorType shift_value;

    }; // ShiftResidual
  }
}


#endif // ifndef __pinocchio_ceres_shift_residual_hpp__
