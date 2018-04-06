#pragma once

#include <stdexcept>
#include <string>

//Base Exceptions

class RuntimeException : public std::runtime_error {
public:
	RuntimeException(const std::string& message = "")
		: runtime_error(message) {}
};

class LogicException : public std::logic_error {
public:
	LogicException(const std::string& message = "")
		: logic_error(message) {}
};

// Runtime Exceptions

class LoadException : public RuntimeException {
public:
	LoadException(const std::string& message = "")
		: RuntimeException(message) {}
};

class SaveException : public RuntimeException {
public:
	SaveException(const std::string& message = "")
		: RuntimeException(message) {}
};

class OperationException : public RuntimeException {
public:
	OperationException(const std::string& message = "")
		: RuntimeException(message) {}
};

class ParameterException : public RuntimeException {
public:
	ParameterException(const std::string& message = "", const std::string& parameter = "")
		: RuntimeException(message)
	{
		Parameter = parameter;
	}
	//The parameter causing the exception.
	std::string Parameter;
};

// Logic Exceptions

class NotImplementedException : public LogicException {
public:
	NotImplementedException(const std::string& message = "Method not implemented yet!")
		: LogicException(message) {}
};
