/*
 * MIT License
 *
 * Copyright (c) 2020 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*!
 * \page clienttutorial QtProtobuf Client Tutorial
 *
 * This tutorial will show how to use QtProtobuf client with some existing server and specified communication protocol.
 *
 * \subsection clienttutorial_dependencies Add QtProtobuf package to CMake project
 *
 * After you successfully installed/built QtProtobuf (see <a href="https://github.com/semlanik/qtprotobuf">README.md</a>) with complete set of dependencies
 * you may start creation either CMake or qmake project in QtCreator. Now we need to tell our project about QtProtobuf.
 * Add following line in your CMakeLists.txt to add QtProtobuf as project dependency:
 *
 * \code
 * find_package(QtProtobuf COMPONENTS ProtobufGenerator Protobuf Grpc REQUIRED)
 * ...
 * target_link_libraries(clienttutorial PRIVATE QtProtobuf::Grpc QtProtobuf::Protobuf)
 * \endcode
 * At this point you will have full access to QtProtobuf libraries and macroses.
 *
 * \subsection clienttutorial_generation Generate code
 *
 * Let's imagine you have echo sever with following protocol definition saved to tutorial.proto:
 * \code
 * syntax="proto3";
 *
 * package qtprotobuf.tutorial;
 *
 * message EchoRequest {
 *   string message = 1;
 * }
 *
 * message EchoResponse {
 *   string message = 1;
 * }
 *
 * service EchoService {
 *   rpc Echo(EchoRequest) returns (EchoResponse);
 * }
 * \endcode
 *
 * Integration with your Qt application starts at point when you add protobuf and gRPC generation for .proto filed. Add \ref cmake_qt6_protobuf_generate function to your CMakeLists.txt as following:
 *
 * \code
 * ...
 * add_executable(clienttutorial main.cpp qml.qrc) #Add QtProtobuf generator rules after CMake target defined
 *
 * qt6_protobuf_generate(TARGET clienttutorial QML TRUE PROTO_FILES tutorial.proto)
 * ...
 * \endcode
 *
 * We specified `QML TRUE` to have generated classes visibled in QML. At this point we have direct access to all classes specified in tutorial.proto from C++ and QML contexts.
 *
 * To use them in C++ you need to include tutorial.qpb.h to get all messages defined in tutorial.proto accessible in C++ context:
 * \code
 * #include "tutorial.qpb.h"
 * \endcode
 *
 * To access messages from QML code add appropriate import of protobuf package:
 * \code
 * import qtprotobuf.tutorial 1.0
 * \endcode
 *
 * \note All QML imports by default generated with version 1.0.
 *
 * \subsection clienttutorial_cpp Implement client-side business logic
 *
 * Now lets go deeper and try to interact with server using our gRPC API. For server communication we will have C++ model, that implements business
 * logic and encapsulates QtGrpc client for communication purpose:
 *
 * \code
 * #include "tutorial.qpb.h"
 * #include "tutorial_grpc.qpb.h"
 *
 * class EchoClientEngine : public QObject
 * {
 *     Q_OBJECT
 *     Q_PROPERTY(qtprotobuf::tutorial::EchoResponse *response READ response CONSTANT)
 * public:
 *     explicit EchoClientEngine(QObject *parent = nullptr);
 *
 *     Q_INVOKABLE void request(qtprotobuf::tutorial::EchoRequest *req);
 *
 *     qtprotobuf::tutorial::EchoResponse *response() const
 *     {
 *         return m_response.get();
 *     }
 *
 * private:
 *     std::unique_ptr<qtprotobuf::tutorial::EchoServiceClient> m_client;
 *     std::unique_ptr<qtprotobuf::tutorial::EchoResponse> m_response;
 * };
 *
 * \endcode
 *
 * Here we specify our interface to communicate to QML part.
 *
 *  - `response` property provides recent response from server. We made it CONSTANT because pointer it selves don't need any modifications.
 *  - `request` invocable function expects pointer to EchoRequest, that will be utilized to call Echo remote procedure.
 *  - `m_client` keeps pointer to generated EchoServerClient.
 *
 * \note This implementation is one of possible usage of client-side grpc API.
 *
 *
 * Definition part of EchoClientEngine includes two main points:
 *
 *  1. Initialization of gRPC channel and attaching our EchoServiceClient to it.
 *  2. Implement request call
 *
 * For gRPC channel we will use Http2 protocol without credentials. Simply create new channel with \ref QtProtobuf::QGrpcInsecureChannelCredentials "QGrpcInsecureChannelCredentials" / \ref QtProtobuf::QGrpcInsecureCallCredentials "QGrpcInsecureCallCredentials"
 * and pass it as parameter to \ref QtProtobuf::QAbstractGrpcClient::attachChannel "attachChannel" call right in constructor:
 *
 * \code
 * EchoClientEngine::EchoClientEngine(QObject *parent) : QObject(parent), m_client(new EchoServiceClient), m_response(new EchoResponse)
 * {
 *     m_client->attachChannel(std::shared_ptr<QAbstractGrpcChannel>(new QGrpcHttp2Channel(QUrl("http://localhost:65000"),
 *                                                                                         QGrpcInsecureChannelCredentials()|QGrpcInsecureCallCredentials())));
 * }
 * \endcode
 *
 * Request function is very simple only utilizes generated Echo method, one of possible implementations as below:
 *
 * \code
 * void EchoClientEngine::request(qtprotobuf::tutorial::EchoRequest *req)
 * {
 *     m_client->Echo(*req, m_response.get());
 * }
 * \endcode
 *
 * Let's stop at parameters, that we pass to `Echo` method.
 * First parameter is request. Methods generated by QtProtobuf pass only reference to method argument, that's why we derefence it and provide as value.
 * Second parameter is pointer to response that will be modified when server response will be received. Important point here is that responce value passed
 * as QPointer and its time of life is controlled by grpc client. In case if pointer was removed client will ignore server response silently. So from this
 * perspective QtGrpc provides to you async response safety.
 *
 * We complete our model implementation, and it's time to make it visible in QML context. Simply do it in main:
 *
 * \code
 * ...
 * #include <QtProtobufTypes>
 *
 * #include "echoclientengine.h
 * ...
 * int main(int argc, char *argv[])
 * {
 * ... //Before load QML
 *     qRegisterProtobufTypes();
 *     qmlRegisterSingletonType<EchoClientEngine>("qtprotobuf.tutorial", 1, 0, "EchoClientEngine", [](QQmlEngine *engine, QJSEngine *){
 *         static EchoClientEngine echoEngine;
 *         engine->setObjectOwnership(&echoEngine, QQmlEngine::CppOwnership);
 *         return &echoEngine;
 *     });
 * ...
 * }
 * \endcode
 *
 * It's important to call `qRegisterProtobufTypes` to register all QtProtobuf types including generated user types.
 * In our case it's types generated from tutorial.proto.
 *
 * \subsection clienttutorial_qml Implement client-side UI and interact to business logic
 * Let's move to presentation part. Of course it's better to use QML when we describe our project UI:
 *
 * \code
 * import QtQuick 2.12
 * import QtQuick.Window 2.12
 *
 * import QtQuick.Controls 2.12
 *
 * import qtprotobuf.tutorial 1.0
 *
 * Window {
 * ...
 *     EchoRequest {
 *         id: request
 *         message: messageInput.text
 *     }
 * ...
 *     TextField {
 *         id: messageInput
 *         anchors.verticalCenter: parent.verticalCenter
 *         width: 400
 *         onAccepted: {
 *             echoEngine.request(request);
 *             text = ""
 *         }
 *     }
 * ...
 *     Text {
 *         anchors.verticalCenter: parent.verticalCenter
 *         text: echoEngine.response.message
 *     }
 * }
 * \endcode
 *
 * After cut off all fancy QML stuff, only QtProtobuf related things are left. Its important to do not forget import of our freshly generated
 * qtprotobuf.tutorial package. At this point we have access to any message inside qtprotobuf.tutorial package. So we create view model right in QML code
 * by adding `EchoRequest` object and bind `message` property to messageInput text.
 * \note It's not mandatory and in some cases may cause often field update, but in out case it's just tutorial ¯\\_(ツ)_/¯.
 *
 * Only thing left is to call our request method. We pass request that already contains text, that we would like to send to server. Result will be received
 * and written to echoEngine.response.message property. We bound it to appropriate text field, that will be updated "immediately".
 * *
 * It's time for testing! For tests you may use test server from projects `examples/tutorial/tutorialserver` folder. Run "build_and_run.sh" script in
 * `examples/tutorial/tutorialserver` folder and enjoy echo functionality of your QtProtobuf EchoService client UI.
 *
 * \subsection clienttutorial_feedback Feedback
 * Please provide feedback using this <a href="https://github.com/semlanik/qtprotobuf/issues/116">issue</a> in project bug tracker.
 *
 * \subsection clienttutorial_references References
 * <a href="https://github.com/semlanik/qtprotobuf/tree/master/examples/clienttutorial">Complete tutorial code</a>
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QtProtobufTypes>

#include "echoclientengine.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    qRegisterProtobufTypes();
    qmlRegisterSingletonType<EchoClientEngine>("qtprotobuf.tutorial", 1, 0, "EchoClientEngine", [](QQmlEngine *engine, QJSEngine *) -> QObject *{
        static EchoClientEngine echoEngine;
        engine->setObjectOwnership(&echoEngine, QQmlEngine::CppOwnership);
        return &echoEngine;
    });

    engine.load(url);

    return app.exec();
}
