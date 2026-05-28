#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

namespace Dev
{
	namespace TraceDev
	{
		inline bool DevSweepSingleByChannel(const UWorld* world, FHitResult& result, const FVector& start, const FVector& end, const FQuat& rotation, const ECollisionChannel collisionChannel, const FCollisionShape& shape, const FCollisionQueryParams& params)
		{
#if WITH_EDITOR
			check(IsValid(world));
			bool traceResult = world->SweepSingleByChannel(result, start, end, rotation, collisionChannel, shape, params);

			//common:
			const float totalLength = (start - end).Length();
			const float traceLength = traceResult ? (start - result.ImpactPoint).Length() : totalLength;
			const auto dir = traceResult ? (result.ImpactPoint - start).GetUnsafeNormal() : (end - start).GetUnsafeNormal();

			if (traceResult)
			{
				DrawDebugPoint(world, result.ImpactPoint, 5, FColor::Green, false, 5);
			}

			if (shape.IsBox())
			{
				//assume the box is uniform
				for (float i = 0; i < totalLength; i += shape.Box.HalfExtentX)
				{
					DrawDebugBox(world, start + dir * i, shape.GetExtent(),
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 0);
				}
				return traceResult;
			}

			if (shape.IsSphere())
			{
				for (float i = 0; i < totalLength; i += shape.Sphere.Radius * 2)
				{
					const auto center = start + i * dir;
					DrawDebugSphere(world, center, shape.Sphere.Radius, 8,
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 0);
				}
				return traceResult;
			}

			if (shape.IsCapsule())
			{
				const auto  halfHeight = shape.GetCapsuleHalfHeight();
				const auto radius = shape.GetCapsuleRadius();

				for (float i = 0; i < totalLength; i += shape.Sphere.Radius * 2)
				{
					const auto center = start + i * dir;
					DrawDebugCapsule(world, center, halfHeight, radius, rotation,
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 0);
				}
				return traceResult;
			}

			check(false);//make sure to cover all shapes
			return traceResult;
#else
			return world->SweepSingleByChannel(result, start, end, rotation, collisionChannel, shape, params);
#endif // WITH_EDITOR
		}

		inline bool DevLineTraceSingleByChannel(const UWorld* world, FHitResult& result, const FVector& start, const FVector& end, const ECollisionChannel channel, const FCollisionQueryParams& params)
		{
#if WITH_EDITOR
			check(IsValid(world));
			bool traceResult = world->LineTraceSingleByChannel(result, start, end, channel, params);

			if (traceResult)
			{
				DrawDebugDirectionalArrow(world, start, result.ImpactPoint, 1, FColor::Green, false, 5, 5);
				DrawDebugDirectionalArrow(world, result.ImpactPoint, end, 1, FColor::Red, false, 5, 5);
				DrawDebugPoint(world, result.ImpactPoint, 5, FColor::Green, false, 5, 5);
			}
			else
			{
				DrawDebugDirectionalArrow(world, start, end, 1, FColor::Red, false, 5, 5);
			}
			return traceResult;
#else
			return world->LineTraceSingleByChannel(result, start, end, channel, params);
#endif
		}
		inline bool DevSweepTestByChannel(const UWorld* world, const FVector& start, const FVector& end, const FQuat& rotation, const ECollisionChannel collisionChannel, const FCollisionShape& shape, const FCollisionQueryParams& params)
		{
#if WITH_EDITOR
			check(IsValid(world));
			FHitResult result;
			bool traceResult = world->SweepSingleByChannel(result, start, end, rotation, collisionChannel, shape, params);

			//common:
			const float totalLength = (start - end).Length();
			const float traceLength = traceResult ? (start - result.ImpactPoint).Length() : totalLength;
			const auto dir = traceResult ? (result.ImpactPoint - start).GetUnsafeNormal() : (end - start).GetUnsafeNormal();

			if (shape.IsBox())
			{
				//assume the box is uniform
				for (float i = 0; i < totalLength; i += shape.Box.HalfExtentX)
				{
					DrawDebugBox(world, start + dir * i, shape.GetExtent(),
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 5);
				}
				return traceResult;
			}

			if (shape.IsSphere())
			{
				for (float i = 0; i < totalLength; i += shape.Sphere.Radius * 2)
				{
					const auto center = start + i * dir;
					DrawDebugSphere(world, center, shape.Sphere.Radius, 8,
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 5);
				}
				return traceResult;
			}

			if (shape.IsCapsule())
			{
				const auto  halfHeight = shape.GetCapsuleHalfHeight();
				const auto radius = shape.GetCapsuleRadius();

				for (float i = 0; i < totalLength; i += shape.Sphere.Radius * 2)
				{
					const auto center = start + i * dir;
					DrawDebugCapsule(world, center, halfHeight, radius, rotation,
						i < traceLength ? FColor::Red : FColor::Green, false, 5, 5);
				}
				return traceResult;
			}

			check(false);//make sure to cover all shapes
			return traceResult;
#else
			return world->SweepTestByChannel(start, end, rotation, collisionChannel, shape, params);
#endif  // WITH_EDITOR
		}
	}
}
