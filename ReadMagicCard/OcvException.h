#pragma once

#include <stdexcept>
#include <string>

//Base Exceptions

class OcvRuntimeException : public std::runtime_error {
public:
	OcvRuntimeException(const std::string& message = "")
		: runtime_error(message) {}
};

class OcvLogicException : public std::logic_error {
public:
	OcvLogicException(const std::string& message = "")
		: logic_error(message) {}
};

//Ocv Runtime Exceptions

class OcvLoadException : public OcvRuntimeException {
public:
	OcvLoadException(const std::string& message = "")
		: OcvRuntimeException(message) {}
};

class OcvSaveException : public OcvRuntimeException {
public:
	OcvSaveException(const std::string& message = "")
		: OcvRuntimeException(message) {}
};

class OcvOperationException : public OcvRuntimeException {
public:
	OcvOperationException(const std::string& message = "")
		: OcvRuntimeException(message) {}
};

class OcvParameterException : public OcvRuntimeException {
public:
	OcvParameterException(const std::string& message = "", const std::string& parameter = "")
		: OcvRuntimeException(message)
	{
		Parameter = parameter;
	}
	//The parameter causing the exception.
	std::string Parameter;
};

//Ocv Logic Exceptions

class OcvNotImplementedException : public OcvLogicException {
public:
	OcvNotImplementedException(const std::string& message = "Method not implemented yet!")
		: OcvLogicException(message) {}
};
