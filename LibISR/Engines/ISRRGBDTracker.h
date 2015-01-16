#pragma once
#include "ISRTracker.h"

namespace LibISR
{
	namespace Engine
	{
		class ISRRGBDTracker : public ISRTracker
		{
		private:

			// the current accepted tracker's state
			// incremental change of poses will always
			// be applied to this state
			Objects::ISRTrackingState * accpetedState;

			// temp tracker state after applying the incremental pose change
			// energy function is always evaluated on this set of poses
			Objects::ISRTrackingState * tempState;

			// pointer to the current set of shapes
			Objects::ISRShapeUnion *shapeUnion;

			// pointer to the current frame
			Objects::ISRFrame *frame;

			// number of objects
			int nObjects;

			// size of the gradient
			int ATb_Size; // (6*nObjects)

			// size of the Hessian
			int ATA_size; // (Atb_size^2)

		protected:

			// Hessian approximated with JTJ
			float* ATA_host;

			// gradient
			float* ATb_host;

			// evaluate the energy given current poses and shapes
			// the poses are always taken from tmpPoses
			virtual void evaluateEnergy(float *energy, Objects::ISRTrackingState * trackerState) = 0;

			// compute the Hessian and the Jacobian given the current poses and shape
			// the poses are always taken from tmpPoses
			virtual void computeJacobianAndHessian(float *gradient, float *hessian, Objects::ISRTrackingState * trackerState) const = 0;

		public:

			int numParameters() const { return ATb_Size; }
			int numObjects() const { return nObjects;  };

			// evaluation point for LM optimization
			class EvaluationPoint
			{
			protected:
				float cacheEnergy;
				float *cacheNabla;
				float *cacheHessian;

				Objects::ISRTrackingState * mState;
				const ISRRGBDTracker *mParent;

				void computeGradients(bool requiresHessian);
				
			public:
				float energy(){ return cacheEnergy; };
				
				const float* nabla_energy(){if (cacheNabla == NULL) computeGradients(false); return cacheNabla; }
				const float* hessian_GN() { if (cacheHessian == NULL) computeGradients(true); return cacheHessian; }
			
				const Objects::ISRTrackingState* getState() const { return mState; }

				EvaluationPoint(Objects::ISRTrackingState * trackerState, const ISRRGBDTracker *f_parent);
				~EvaluationPoint(void)
				{
					delete mState;
					if (cacheNabla != NULL) delete[] cacheNabla;
					if (cacheHessian != NULL) delete[] cacheHessian;
				}
			};

			EvaluationPoint* evaluateAt(Objects::ISRTrackingState * trackerState) const
			{
				return new EvaluationPoint(trackerState, this);
			}

			void  TrackObjects(Objects::ISRFrame *frame, Objects::ISRShapeUnion *shapeUnion, Objects::ISRTrackingState *trackerState) = 0;

			ISRRGBDTracker(int nObjs, bool useGPU);
			~ISRRGBDTracker();
		};

	}
}

