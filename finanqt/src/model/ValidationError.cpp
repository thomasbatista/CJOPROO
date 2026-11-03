#include "model/ValidationError.h"

ValidationError::ValidationError(const QString &message)
    : std::runtime_error(message.toStdString()), m_qMessage(message) {}

ValidationError::ValidationError(const char *message)
    : std::runtime_error(message ? message : ""),
      m_qMessage(QString::fromUtf8(message ? message : "")) {}

ValidationError::ValidationError(const std::string &message)
    : std::runtime_error(message), m_qMessage(QString::fromStdString(message)) {}

QString ValidationError::qWhat() const {
    return m_qMessage;
}
