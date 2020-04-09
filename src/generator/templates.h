/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>, Tatyana Borisova <tanusshhka@mail.ru>
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

#pragma once

#include <unordered_map>
#include <string>
#include <google/protobuf/descriptor.h>

namespace QtProtobuf {
namespace generator {

/*!
 * \ingroup generator
 * \private
 * \brief The Templates class
 */
class Templates {
public:
    static const std::vector<std::string> ListOfQmlExeptions;
    static const char *ProtoSufix;
    static const char *DefaultProtobufIncludesTemplate;
    static const char *QmlProtobufIncludesTemplate;
    static const char *GlobalEnumClassNameTemplate;
    static const char *PreambleTemplate;
    static const char *DisclaimerTemplate;
    static const char *InternalIncludeTemplate;
    static const char *ExternalIncludeTemplate;
    static const char *GlobalEnumIncludeTemplate;
    static const char *UsingQtProtobufNamespaceTemplate;
    static const char *ManualRegistrationDeclaration;
    static const char *ManualRegistrationComplexTypeDefinition;
    static const char *ManualRegistrationGlobalEnumDefinition;
    static const char *ComplexGlobalEnumFieldRegistrationTemplate;
    static const char *ComplexListTypeUsingTemplate;
    static const char *MapTypeUsingTemplate;
    static const char *MessageMapTypeUsingTemplate;
    static const char *EnumTypeRepeatedTemplate;
    static const char *NamespaceTemplate;
    static const char *UsingNamespaceTemplate;
    static const char *NonProtoClassDefinitionTemplate;
    static const char *ProtoClassDeclarationTemplate;
    static const char *ProtoClassDefinitionTemplate;
    static const char *ConstructorHeaderTemplate;
    static const char *ClassDefinitionTemplate;
    static const char *QObjectMacro;
    static const char *PropertyTemplate;
    static const char *NonScriptablePropertyTemplate;
    static const char *NonScriptableAliasPropertyTemplate;
    static const char *MessagePropertyTemplate;
    static const char *QmlListPropertyTemplate;
    static const char *MemberTemplate;
    static const char *ComplexMemberTemplate;
    static const char *EnumMemberTemplate;
    static const char *PublicBlockTemplate;
    static const char *PrivateBlockTemplate;
    static const char *EnumDefinitionTemplate;
    static const char *EnumFieldTemplate;
    static const char *ProtoConstructorTemplate;
    static const char *ProtoConstructorDefinitionTemplate;
    static const char *ConstructorTemplate;
    static const char *QObjectConstructorTemplate;
    static const char *CopyConstructorDeclarationTemplate;
    static const char *MoveConstructorDeclarationTemplate;
    static const char *CopyConstructorDefinitionTemplate;
    static const char *MoveConstructorDefinitionTemplate;
    static const char *EmptyCopyConstructorDefinitionTemplate;
    static const char *EmptyMoveConstructorDefinitionTemplate;
    static const char *DeletedCopyConstructorTemplate;
    static const char *DeletedMoveConstructorTemplate;
    static const char *CopyFieldTemplate;
    static const char *CopyComplexFieldTemplate;
    static const char *MoveMessageFieldTemplate;
    static const char *MoveComplexFieldTemplate;
    static const char *MoveComplexFieldConstructorTemplate;
    static const char *MoveFieldTemplate;
    static const char *EnumMoveFieldTemplate;
    static const char *AssignmentOperatorDeclarationTemplate;
    static const char *AssignmentOperatorDefinitionTemplate;
    static const char *EmptyAssignmentOperatorDefinitionTemplate;
    static const char *AssignmentOperatorReturnTemplate;
    static const char *MoveAssignmentOperatorDeclarationTemplate;
    static const char *MoveAssignmentOperatorDefinitionTemplate;
    static const char *EmptyMoveAssignmentOperatorDefinitionTemplate;
    static const char *EqualOperatorDeclarationTemplate;
    static const char *EqualOperatorDefinitionTemplate;
    static const char *EmptyEqualOperatorDefinitionTemplate;
    static const char *EqualOperatorPropertyTemplate;
    static const char *EqualOperatorMessagePropertyTemplate;
    static const char *NotEqualOperatorDeclarationTemplate;
    static const char *NotEqualOperatorDefinitionTemplate;
    static const char *GetterPrivateMessageDeclarationTemplate;
    static const char *GetterPrivateMessageDefinitionTemplate;
    static const char *GetterMessageDeclarationTemplate;
    static const char *GetterMessageDefinitionTemplate;
    static const char *GetterTemplate;
    static const char *NonScriptableGetterTemplate;
    static const char *GetterContainerExtraDeclarationTemplate;
    static const char *GetterContainerExtraDefinitionTemplate;
    static const char *GetterQmlListDeclarationTemplate;
    static const char *GetterQmlListDefinitionTemplate;

    static const char *SetterPrivateTemplateDeclarationMessageType;
    static const char *SetterPrivateTemplateDefinitionMessageType;
    static const char *SetterTemplateDeclarationMessageType;
    static const char *SetterTemplateDefinitionMessageType;
    static const char *SetterTemplateDeclarationComplexType;
    static const char *SetterTemplateDefinitionComplexType;
    static const char *SetterTemplate;
    static const char *NonScriptableSetterTemplate;
    static const char *SignalsBlockTemplate;
    static const char *SignalTemplate;
    static const char *FieldsOrderingContainerTemplate;
    static const char *FieldOrderTemplate;
    static const char *EnumTemplate;
    static const char *SimpleBlockEnclosureTemplate;
    static const char *SemicolonBlockEnclosureTemplate;
    static const char *EmptyBlockTemplate;
    static const char *PropertyInitializerTemplate;
    static const char *PropertyEmptyInitializerTemplate;
    static const char *MessagePropertyInitializerTemplate;
    static const char *MessagePropertyEmptyInitializerTemplate;
    static const char *ConstructorContentTemplate;
    static const char *DeclareMetaTypeTemplate;
    static const char *DeclareMetaTypeListTemplate;
    static const char *DeclareMessageMetaTypeTemplate;
    static const char *DeclareComplexListTypeTemplate;
    static const char *DeclareComplexQmlListTypeTemplate;
    static const char *RegisterMetaTypeDefaultTemplate;
    static const char *RegisterMetaTypeTemplate;
    static const char *RegisterMetaTypeTemplateNoNamespace;
    static const char *RegisterQmlListPropertyMetaTypeTemplate;
    static const char *QEnumTemplate;
    static const char *MapSerializationRegisterTemplate;
    static const char *RegisterSerializersTemplate;
    static const char *RegisterEnumSerializersTemplate;
    static const char *RegistrarTemplate;
    static const char *EnumRegistrarTemplate;
    static const char *QmlRegisterTypeTemplate;
    static const char *QmlRegisterTypeUncreatableTemplate;
    //Service templates
    static const char *ClientConstructorDefinitionTemplate;

    static const char *ClientMethodDeclarationSyncTemplate;
    static const char *ClientMethodDeclarationAsyncTemplate;
    static const char *ClientMethodDeclarationAsync2Template;
    static const char *ServerMethodDeclarationTemplate;

    static const char *ClientMethodDefinitionSyncTemplate;
    static const char *ClientMethodDefinitionAsyncTemplate;
    static const char *ClientMethodDefinitionAsync2Template;

    //Streaming
    static const char *ClientMethodSignalDeclarationTemplate;
    static const char *ClientMethodServerStreamDeclarationTemplate;
    static const char *ClientMethodServerStream2DeclarationTemplate;
    static const char *ClientMethodServerStreamDefinitionTemplate;
    static const char *ClientMethodServerStream2DefinitionTemplate;

    static const char *ListSuffix;
    static const char *ProtoFileSuffix;
    static const char *GrpcFileSuffix;
    static const char *EnumClassSuffix;

    static const std::unordered_map<::google::protobuf::FieldDescriptor::Type, std::string> TypeReflection;
};

} //namespace generator
} //namespace QtProtobuf
