#include <znc/Modules.h>

class ExampleModule : public CModule {
  public:
    MODCONSTRUCTOR(ExampleModule) {}

    void OnModCommand(const CString& command) override {
        PutModule("command received: " + command);
    }
};

MODULEDEFS(ExampleModule, "Example external module")