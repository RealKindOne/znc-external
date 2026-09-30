#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include "znctest.h"

namespace znc_inttest {
namespace {

TEST_F(ZNCTest, ExampleModule) {
    auto znc = Run();
    auto ircd = ConnectIRCd();
    auto client = LoginClient();

    QFile source(QStringLiteral(EXTERNAL_SOURCE_DIR) +
                 QStringLiteral("/modules/example.cpp"));

    ASSERT_TRUE(source.open(QIODevice::ReadOnly | QIODevice::Text))
        << source.errorString().toStdString();

    const QString moduleSource =
        QString::fromUtf8(source.readAll());

    InstallModule(QStringLiteral("example.cpp"), moduleSource);

    client.Write("znc loadmod example");
    client.ReadUntil("Loaded module example");

    client.Write("PRIVMSG *example :hello");
    client.ReadUntil("command received: hello");
}

}  // namespace
}  // namespace znc_inttest