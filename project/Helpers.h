#pragma once

template<typename Info, typename Type>
struct IsType
{
	IsType(Type type): typeToCheck(type) {}
	
	bool operator() (const Info& info) const
	{
		return info.Type == typeToCheck;
	}

	Type typeToCheck;
};

struct IsCloser
{
	IsCloser(const Elite::Vector2& v) : point(v){}

	template <typename Info>
	bool operator() (const Info& lhs, const Info& rhs) const
	{
		return Elite::DistanceSquared(point, lhs.Location) < Elite::DistanceSquared(point, rhs.Location);
	}

	bool operator()(const HouseInfo& lhs, const HouseInfo& rhs) const
	{
		return Elite::DistanceSquared(point, lhs.Center) < Elite::DistanceSquared(point, rhs.Center);
	}

	bool operator() (const Elite::Vector2& lhs, Elite::Vector2& rhs) const
	{
		return Elite::DistanceSquared(point, lhs) < Elite::DistanceSquared(point, rhs);
	}

	Elite::Vector2 point;
};

struct IsFurther
{
	IsFurther(const Elite::Vector2& v) : point(v) {}

	template <typename Info>
	bool operator() (const Info& lhs, const Info& rhs) const
	{
		return Elite::DistanceSquared(point, lhs.Location) > Elite::DistanceSquared(point, rhs.Location);
	}

	bool operator() (const Elite::Vector2& lhs, Elite::Vector2& rhs) const
	{
		return Elite::DistanceSquared(point, lhs) > Elite::DistanceSquared(point, rhs);
	}


	Elite::Vector2 point;
};

struct SameLocation
{
	template<typename Info>
	SameLocation(const Info& rhs) : rhs{ rhs.Location } {};
	SameLocation(const Elite::Vector2& location) : rhs{ location } {};

	template<typename Info>
	bool operator() (const Info& lhs) const
	{
		return lhs.Location == rhs;
	}

	bool operator()(const HouseInfo& lhs) const
	{
		return lhs.Center == rhs;
	}


	bool operator() (const Elite::Vector2& lhs) const
	{
		return lhs == rhs;
	}

	Elite::Vector2 rhs;
	
};