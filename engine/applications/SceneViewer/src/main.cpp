// Copryright (C) 2021 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickWindow>
#include <QSGRendererInterface>

#include <QQuickStyle>

#include "ComponentReflector.hpp"
#include "StormKitView.hpp"

#ifdef Q_OS_WINDOWS
    #include <dwmapi.h>

static constexpr auto DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;
static constexpr auto DWMWA_USE_IMMERSIVE_DARK_MODE             = 20;

typedef LONG NTSTATUS, *PNTSTATUS;
    #define STATUS_SUCCESS (0x00000000)

typedef NTSTATUS(WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

inline RTL_OSVERSIONINFOW GetRealOSVersion() {
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (hMod) {
        RtlGetVersionPtr fxPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
        if (fxPtr != nullptr) {
            RTL_OSVERSIONINFOW rovi  = { 0 };
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            if (STATUS_SUCCESS == fxPtr(&rovi)) { return rovi; }
        }
    }
    RTL_OSVERSIONINFOW rovi = { 0 };

    FreeLibrary(hMod);

    return rovi;
}
#endif

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    auto app = QGuiApplication { argc, argv };
    QQuickStyle::setStyle("Fusion");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::VulkanRhi);

    auto engine = QQmlApplicationEngine {};

    qmlRegisterUncreatableType<StormKitScene>("StormKit.Scene",
                                              1,
                                              0,
                                              "StormKitScene",
                                              "Not creatable in Qml.");
    qmlRegisterType<StormKitView>("StormKit.View", 1, 0, "StormKitView");
    qmlRegisterType<ComponentReflector>("StormKit.Component.Reflector", 1, 0, "ComponentReflector");

    const auto url = QUrl { QStringLiteral("qrc:/qml/main.qml") };

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](auto *obj, const auto &obj_url) {
            if (!obj && url == obj_url) QCoreApplication::exit(EXIT_FAILURE);
        },
        Qt::QueuedConnection);

    engine.load(url);

#ifdef Q_OS_WINDOWS
    auto root_object = engine.rootObjects().first();

    auto attribute = DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1;
    if (GetRealOSVersion().dwBuildNumber >= 18985) attribute = DWMWA_USE_IMMERSIVE_DARK_MODE;

    auto handle = static_cast<QQuickWindow *>(root_object)->winId();
    auto set    = 1;
    DwmSetWindowAttribute(HWND(handle), attribute, &set, sizeof(int));
#endif

    return app.exec();
}
