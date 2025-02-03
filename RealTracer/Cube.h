#pragma once
//#include "Hittable.h"
//class Cube :
//    public Hittable
//{
//public:
//	Cube(Vec3 position, Vec3 size);
//
//	xs::batch_bool<float> Intersect(const RayGroup& ray, IntervalGroup rayT, HitInfoGroup& outHit) const override;
//	void SetPosition(Vec3 _position)
//	{
//		m_position = _position;
//		UpdateIntervals();
//	};
//	void SetSize(Vec3 _size)
//	{
//		m_size = _size;
//		UpdateIntervals();
//	}
//private:
//	void UpdateIntervals();
//	xs::batch_bool<float> IntersectsAxis(xs::batch<float> rayAxisDirection, xs::batch<float> rayAxisOrigin, IntervalGroup rayInterval, IntervalGroup cubeAxisInterval) const;
//	Vec3 m_position;
//	Vec3 m_size;
//	IntervalGroup m_xInterval, m_yInterval, m_zInterval;
//};
//
