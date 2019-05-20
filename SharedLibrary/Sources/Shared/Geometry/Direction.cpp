#include "Direction.hpp"

#include "Shared/Types.hpp"

namespace uf {
	Direction VectToDirection(rpos vector) {
		return Direction(
			(vector.x > 0 ? Direction::EAST  : Direction::NONE) |
			(vector.x < 0 ? Direction::WEST  : Direction::NONE) |
			(vector.y > 0 ? Direction::SOUTH : Direction::NONE) |
			(vector.y < 0 ? Direction::NORTH : Direction::NONE) |
			(vector.z > 0 ? Direction::UP    : Direction::NONE) |
			(vector.z < 0 ? Direction::DOWN  : Direction::NONE)
		);
	}
	Direction VectToDirection(vec2i vector) {
		return Direction(
			(vector.x > 0 ? Direction::EAST  : Direction::NONE) |
			(vector.x < 0 ? Direction::WEST  : Direction::NONE) |
			(vector.y > 0 ? Direction::SOUTH : Direction::NONE) |
			(vector.y < 0 ? Direction::NORTH : Direction::NONE)
		);
	}

	int log2(Direction dir) {
		int offset = 0;
		while (char(dir) >> ++offset) {}
		return offset;
	}
	rpos DirectionToVect(Direction direction) {
		return {bool(direction & Direction::EAST)  - bool(direction & Direction::WEST),
		        bool(direction & Direction::SOUTH) - bool(direction & Direction::NORTH),
		        bool(direction & Direction::UP)    - bool(direction & Direction::DOWN)};
	}
	Direction operator |(Direction l, Direction r) {
		return Direction(char(l) | char(r));
	}
	Direction operator &(Direction l, Direction r) {
		return Direction(char(l) & char(r));
	}
}