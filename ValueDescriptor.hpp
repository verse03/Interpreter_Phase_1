// Given by professor: include guard and includes
#ifndef VALUE_DESCRIPTOR_HPP
#define VALUE_DESCRIPTOR_HPP

#include <cstdint>
#include <string>

// Phase 3 - base class for every runtime value. Given by professor
class ValueDescriptor {
public:
    enum class Type {
        integer,
        doubleValue,
        boolean,
        string
    };

    virtual ~ValueDescriptor() = default;

    [[nodiscard]] virtual Type type() const = 0;
    [[nodiscard]] virtual std::string typeName() const = 0;
    [[nodiscard]] virtual std::string toString() const = 0;
    [[nodiscard]] virtual ValueDescriptor *clone() const = 0;
};

// Phase 3 - integer value (int64_t). Given by professor
class IntegerValueDescriptor final : public ValueDescriptor {
public:
    explicit IntegerValueDescriptor(std::int64_t value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] std::int64_t value() const;

private:
    std::int64_t integerValue;
};

// Phase 3 - double value. Given by professor
class DoubleValueDescriptor final : public ValueDescriptor {
public:
    explicit DoubleValueDescriptor(double value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] double value() const;

private:
    double doubleValue;
};

// Phase 3 - boolean value. Given by professor
class BooleanValueDescriptor final : public ValueDescriptor {
public:
    explicit BooleanValueDescriptor(bool value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] bool value() const;

private:
    bool booleanValue;
};

// Phase 3 - string value. Given by professor
class StringValueDescriptor final : public ValueDescriptor {
public:
    explicit StringValueDescriptor(std::string value);

    [[nodiscard]] Type type() const override;
    [[nodiscard]] std::string typeName() const override;
    [[nodiscard]] std::string toString() const override;
    [[nodiscard]] ValueDescriptor *clone() const override;
    [[nodiscard]] const std::string &value() const;

private:
    std::string stringValue;
};

// Given by professor: end of include guard
#endif