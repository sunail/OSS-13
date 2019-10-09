#include "DirectionSet.h"

namespace uf {

constexpr size_t directionToIndex(Direction direction) {
	if (direction == Direction::CENTER)
		return 4;
	EXPECT(char(direction) < 5);
	return char(direction);
}

DirectionSet::DirectionSet(std::list<Direction> directions) {
	Add(directions);
}

void DirectionSet::Add(DirectionSet directions) {
	buffer |= directions.buffer;
}

void DirectionSet::Add(const std::list<Direction> &directions) {
	for (auto direction : directions) {
		if (direction == Direction::NONE)
			continue;
		Direction first, second;
		if (SplitDirection(direction, first, second)) {
			buffer[directionToIndex(first)] = true;
			buffer[directionToIndex(second)] = true;
		} else {
			buffer[directionToIndex(direction)] = true;
		}
	}
}

void DirectionSet::Remove(DirectionSet directions) {
	buffer &= ~directions.buffer;
}

void DirectionSet::Remove(const std::list<Direction> &directions) {
	Remove(DirectionSet(directions));
}

bool DirectionSet::DoesExistOne(DirectionSet directions) const {
	return (buffer & directions.buffer).any();
}

bool DirectionSet::DoesExistOne(const std::list<Direction> &directions) const {
	return DoesExistOne(DirectionSet(directions));
}

bool DirectionSet::DoExistAll(DirectionSet directions) const {
	return (buffer & directions.buffer) == directions.buffer;
}

bool DirectionSet::DoExistAll(const std::list<Direction> &directions) const {
	return DoExistAll(DirectionSet(directions));
}

DirectionSet DirectionSet::Rotate(Direction direction) const {
	size_t shift;
	switch (direction) {
		case Direction::WEST:
			shift = 1;
			break;
		case Direction::NORTH:
			shift = 2;
			break;
		case Direction::EAST:
			shift = 3;
			break;
		default:
			return *this;
	}

	DirectionSet result;

	for (size_t i = 0; i < 3; i++)
		result.buffer[(i + shift) % 4] = buffer[i];
	result.buffer[4] = buffer[4];

	return result;
}

void DirectionSet::Reset() {
	buffer.reset();
}

const std::bitset<5> &DirectionSet::GetBuffer() const { return buffer; }
void DirectionSet::SetBuffer(std::bitset<5> buffer) { this->buffer = buffer; }




DirectionSetFractional::DirectionSetFractional(std::initializer_list<DirectionFractional> fractDirections) {
	Add(fractDirections);
}

void DirectionSetFractional::Add(std::initializer_list<DirectionFractional> fractDirections) {
	for (auto fractDirection : fractDirections) {
		fractions[directionToIndex(fractDirection.direction)] = fractDirection.fraction;
	}
}

void DirectionSetFractional::Remove(std::initializer_list<Direction> directions) {
	for (auto direction : directions) {
		fractions[directionToIndex(direction)] = 0.f;
	}
}

double DirectionSetFractional::GetMaxFraction(std::initializer_list<Direction> directions) const {
	float max = 0.f;
	for (auto direction : directions) {
		if (max < fractions[directionToIndex(direction)])
			max = fractions[directionToIndex(direction)];
	}
	return max;
}

void DirectionSetFractional::Reset() {
	std::fill(fractions.begin(), fractions.end(), 0.f);
}

const std::array<float, 5> &DirectionSetFractional::GetFractions() const { return fractions; }
void DirectionSetFractional::SetFractions(std::array<float, 5> &&fractions) { this->fractions = std::forward<std::array<float, 5>>(fractions); }

}
