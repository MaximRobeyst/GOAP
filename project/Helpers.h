#pragma once

template<typename Info, typename Type>
struct IsType
{
	IsType(Type type): typeToCheck(type) {}
	
	bool operator() (const Info& info)
	{
		return info.Type == typeToCheck;
	}

	Type typeToCheck;
};

template <typename Info>
struct IsCloser
{
	IsCloser(const Elite::Vector2& v) : point(v){}

	bool operator() (const Info& lhs, const Info& rhs)
	{
		return Elite::DistanceSquared(point, lhs.Location) < Elite::DistanceSquared(point, rhs.Location);
	}

	Elite::Vector2 point;
};

template <typename Info>
struct IsFurther
{
	IsFurther(const Elite::Vector2& v) : point(v) {}

	bool operator() (const Info& lhs, const Info& rhs)
	{
		return Elite::DistanceSquared(point, lhs.Location) > Elite::DistanceSquared(point, rhs.Location);
	}

	Elite::Vector2 point;
};