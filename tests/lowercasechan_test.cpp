#include <gtest/gtest.h>

#include <QDir>
#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

#include "znctest.h"

namespace znc_inttest {
namespace {


TEST_F(ZNCTest, LowercaseChanModule) {
    auto znc = Run();
    auto ircd = ConnectIRCd();
    auto client = LoginClient();

    QFile source(QStringLiteral(EXTERNAL_SOURCE_DIR) +
                 QStringLiteral("/modules/lowercasechan.cpp"));

    ASSERT_TRUE(source.open(QIODevice::ReadOnly | QIODevice::Text))
        << source.errorString().toStdString();

    const QString moduleSource =
        QString::fromUtf8(source.readAll());

    InstallModule(QStringLiteral("lowercasechan.cpp"), moduleSource);

    client.Write("znc loadmod lowercasechan");
    client.ReadUntil("Loaded module");

    ircd.Write(":server 001 nick :Hello");

	// Only test JOIN and /whois.
	ircd.Write(":test JOIN #TEST");
	client.ReadUntil("JOIN #test");

	ircd.Write(":server 319 test example :#TEST");
    client.ReadUntil("server 319 test example :#test");

    ircd.Write(":server 319 test example :##Llamas @#aBCd @#XYZ @+#TEST");
    client.ReadUntil("server 319 test example :##llamas @#abcd @#xyz @+#test");
}

}  // namespace
}  // namespace znc_inttest