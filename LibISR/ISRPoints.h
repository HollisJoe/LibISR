#ifndef __ISR_POINTS__
#define __ISR_POINTS__

#include <stdlib.h>

#include "LibISRDefine.h"
#include "MathUtils.h"

namespace LibISR
{
	namespace Objects
	{
		class ISRPoints
		{
		public:
			int isAllocated;
			int count;
			int maxSize;

			Vector3f *data;

			ISRPoints(int maxLength)
			{
				this->maxSize = maxLength;
				data = (Vector3f*)malloc(sizeof(Vector3f)*maxLength);
				this->Clear();
				isAllocated = true;
			}

			void CopyFrom(ISRPoints* inPoints)
			{
				this->count = inPoints->count;
				memcpy(this->data,inPoints->data, inPoints->count*sizeof(Vector3f));
			}

			

			void Clear(unsigned char defaultValue = 0) { memset(data, defaultValue, maxSize * sizeof(Vector3f)); }

			void Free()
			{
				free(data);
				this->isAllocated = false;
			}

			~ISRPoints() { if (isAllocated) this->Free(); }
		};
	}
}

#endif