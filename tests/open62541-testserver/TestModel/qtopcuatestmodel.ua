K��b             8 h t t p : / / o p c f o u n d a t i o n . o r g / U A /   N h t t p : / / q t - p r o j e c t . o r g / Q t O p c U a T e s t M o d e l /           2   @       �    QtTestEnumeration����   QtTestEnumeration    ��������                    QtTestEnumeration   QtTestEnumeration      Unknown��������           FirstOption��������          SecondOption��������          ThirdOption��������          @       �    QtTestStructType����   QtTestStructType    ��������                     QtTestStructType   QtTestStructType      StringMember��������   StringMember         ����         LocalizedTextMember��������   LocalizedTextMember         ����         QualifiedNameMember��������   QualifiedNameMember         ����         Int64ArrayMember��������   Int64ArrayMember                            
EnumMember��������   
EnumMember       �����         NestedStructMember��������   NestedStructMember       �����         NestedStructArrayMember��������   NestedStructArrayMember       �                         @       �    QtInnerTestStructType����   QtInnerTestStructType    ��������                     QtInnerTestStructType   QtInnerTestStructType      DoubleSubtypeMember��������   DoubleSubtypeMember       �����               @       �    QtTestUnionType����   QtTestUnionType    ��������                    QtTestUnionType   QtTestUnionType      Member1��������   Member1         ����         Member2��������   Member2       �����               @       �    QtStructWithOptionalFieldType����   QtStructWithOptionalFieldType    ��������                     QtStructWithOptionalFieldType   QtStructWithOptionalFieldType      MandatoryMember��������             ����         OptionalMember��������             ����              @       �    MyDoubleSubtype����   MyDoubleSubtype    ��������                 @       �    QtAbstractBaseStruct����   QtAbstractBaseStruct    ��������                    QtAbstractBaseStruct   QtAbstractBaseStruct             @       �    QtTestStructWithDiagnosticInfo����   QtTestStructWithDiagnosticInfo    ��������                     QtTestStructWithDiagnosticInfo   QtTestStructWithDiagnosticInfo      DiagnosticInfoMember��������   DiagnosticInfoMember         ����         DiagnosticInfoArrayMember��������   DiagnosticInfoArrayMember                                  @       �    QtTestStructWithDataValue       QtTestStructWithDataValue����                             QtTestStructWithDataValue   QtTestStructWithDataValue      DataValueMember��������   DataValueMember         ����         VariantMember��������   VariantMember         ����        ����          �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �    DecoderTestNodes����   DecoderTestNodes    ��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       �     Default XML����   Default XML   
DefaultXml��������                       �     Default XML����   Default XML   
DefaultXml��������                       �     Default Binary����   Default Binary   DefaultBinary��������                       �     Default XML����   Default XML   
DefaultXml��������                       q     EnumStrings����   EnumStrings    ��������                                       ����   Unknown����   FirstOption����   SecondOption����   ThirdOption                      r    StructWithoutOptionalField       StructWithoutOptionalField   StructWihoutOptionalField                           �����                           s    UnionWithSecondMember����   UnionWithSecondMember    ��������                   �����                           t    QtTestStructWithDiagnosticInfo����   QtTestStructWithDiagnosticInfo    ��������                     ����              
    < Q t T e s t S t r u c t W i t h D i a g n o s t i c I n f o                      u    QtTestStructWithDiagnosticInfo����   QtTestStructWithDiagnosticInfo    ��������                     ����              
    h / / x s : e l e m e n t [ @ n a m e = ' Q t T e s t S t r u c t W i t h D i a g n o s t i c I n f o ' ]                      v    StructWithDiagnosticInfo����   StructWithDiagnosticInfo   StructWithDiagnosticInfo��������                   �����                           w    QtTestStructWithDataValue����   QtTestStructWithDataValue������������                     ����              
    2 Q t T e s t S t r u c t W i t h D a t a V a l u e                      x    QtTestStructWithDataValue����   QtTestStructWithDataValue������������                     ����              
    ^ / / x s : e l e m e n t [ @ n a m e = ' Q t T e s t S t r u c t W i t h D a t a V a l u e ' ]                      y    NestedStruct����   NestedStruct    ��������                   �����                           z    StructWithOptionalField����   StructWithOptionalField    ��������                   �����                           {    UnionWithFirstMember����   UnionWithFirstMember    ��������                   �����                           |    TypeDictionary����   TypeDictionary   TypeDictionary_BinarySchema����   NCollects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/                     ����                 *<opc:TypeDictionary xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:tns="http://qt-project.org/QtOpcUaTestModel/" DefaultByteOrder="LittleEndian" xmlns:opc="http://opcfoundation.org/BinarySchema/" xmlns:ua="http://opcfoundation.org/UA/" TargetNamespace="http://qt-project.org/QtOpcUaTestModel/">
 <opc:Import Namespace="http://opcfoundation.org/UA/"/>
 <opc:StructuredType BaseType="ua:ExtensionObject" Name="QtAbstractBaseStruct"/>
 <opc:StructuredType BaseType="tns:QtAbstractBaseStruct" Name="QtInnerTestStructType">
  <opc:Field TypeName="opc:Double" Name="DoubleSubtypeMember"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="tns:QtAbstractBaseStruct" Name="QtStructWithOptionalFieldType">
  <opc:Field TypeName="opc:Bit" Name="OptionalMemberSpecified"/>
  <opc:Field Length="31" TypeName="opc:Bit" Name="Reserved1"/>
  <opc:Field TypeName="opc:Double" Name="MandatoryMember"/>
  <opc:Field SwitchField="OptionalMemberSpecified" TypeName="opc:Double" Name="OptionalMember"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="tns:QtAbstractBaseStruct" Name="QtTestStructType">
  <opc:Field TypeName="opc:CharArray" Name="StringMember"/>
  <opc:Field TypeName="ua:LocalizedText" Name="LocalizedTextMember"/>
  <opc:Field TypeName="ua:QualifiedName" Name="QualifiedNameMember"/>
  <opc:Field TypeName="opc:Int32" Name="NoOfInt64ArrayMember"/>
  <opc:Field LengthField="NoOfInt64ArrayMember" TypeName="opc:Int64" Name="Int64ArrayMember"/>
  <opc:Field TypeName="tns:QtTestEnumeration" Name="EnumMember"/>
  <opc:Field TypeName="tns:QtInnerTestStructType" Name="NestedStructMember"/>
  <opc:Field TypeName="opc:Int32" Name="NoOfNestedStructArrayMember"/>
  <opc:Field LengthField="NoOfNestedStructArrayMember" TypeName="tns:QtInnerTestStructType" Name="NestedStructArrayMember"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="tns:QtAbstractBaseStruct" Name="QtTestStructWithDataValue">
  <opc:Field TypeName="ua:DataValue" Name="DataValueMember"/>
  <opc:Field TypeName="ua:Variant" Name="VariantMember"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="tns:QtAbstractBaseStruct" Name="QtTestStructWithDiagnosticInfo">
  <opc:Field TypeName="ua:DiagnosticInfo" Name="DiagnosticInfoMember"/>
  <opc:Field TypeName="opc:Int32" Name="NoOfDiagnosticInfoArrayMember"/>
  <opc:Field LengthField="NoOfDiagnosticInfoArrayMember" TypeName="ua:DiagnosticInfo" Name="DiagnosticInfoArrayMember"/>
 </opc:StructuredType>
 <opc:StructuredType BaseType="ua:Union" Name="QtTestUnionType">
  <opc:Field TypeName="opc:UInt32" Name="SwitchField"/>
  <opc:Field SwitchField="SwitchField" TypeName="opc:Int64" SwitchValue="1" Name="Member1"/>
  <opc:Field SwitchField="SwitchField" TypeName="tns:QtInnerTestStructType" SwitchValue="2" Name="Member2"/>
 </opc:StructuredType>
 <opc:EnumeratedType LengthInBits="32" Name="QtTestEnumeration">
  <opc:EnumeratedValue Name="Unknown" Value="0"/>
  <opc:EnumeratedValue Name="FirstOption" Value="1"/>
  <opc:EnumeratedValue Name="SecondOption" Value="2"/>
  <opc:EnumeratedValue Name="ThirdOption" Value="3"/>
 </opc:EnumeratedType>
</opc:TypeDictionary>
                      }     NamespaceUri����   NamespaceUri    ��������                     ����              
    N h t t p : / / q t - p r o j e c t . o r g / Q t O p c U a T e s t M o d e l /                      ~    TypeDictionary����   TypeDictionary   TypeDictionary_XmlSchema����   NCollects the data type descriptions of http://qt-project.org/QtOpcUaTestModel/                     ����                 <xs:schema elementFormDefault="qualified" targetNamespace="http://qt-project.org/QtOpcUaTestModel/Types.xsd" xmlns:tns="http://qt-project.org/QtOpcUaTestModel/Types.xsd" xmlns:ua="http://opcfoundation.org/UA/2008/02/Types.xsd" xmlns:xs="http://www.w3.org/2001/XMLSchema">
 <xs:import namespace="http://opcfoundation.org/UA/2008/02/Types.xsd"/>
 <xs:simpleType name="QtTestEnumeration">
  <xs:restriction base="xs:string">
   <xs:enumeration value="Unknown_0"/>
   <xs:enumeration value="FirstOption_1"/>
   <xs:enumeration value="SecondOption_2"/>
   <xs:enumeration value="ThirdOption_3"/>
  </xs:restriction>
 </xs:simpleType>
 <xs:element type="tns:QtTestEnumeration" name="QtTestEnumeration"/>
 <xs:complexType name="ListOfQtTestEnumeration">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtTestEnumeration" name="QtTestEnumeration" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtTestEnumeration" name="ListOfQtTestEnumeration" nillable="true"/>
 <xs:complexType name="QtAbstractBaseStruct">
  <xs:sequence/>
 </xs:complexType>
 <xs:element type="tns:QtAbstractBaseStruct" name="QtAbstractBaseStruct"/>
 <xs:complexType name="ListOfQtAbstractBaseStruct">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtAbstractBaseStruct" name="QtAbstractBaseStruct" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtAbstractBaseStruct" name="ListOfQtAbstractBaseStruct" nillable="true"/>
 <xs:complexType name="QtInnerTestStructType">
  <xs:complexContent mixed="false">
   <xs:extension base="tns:QtAbstractBaseStruct">
    <xs:sequence>
     <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="DoubleSubtypeMember"/>
    </xs:sequence>
   </xs:extension>
  </xs:complexContent>
 </xs:complexType>
 <xs:element type="tns:QtInnerTestStructType" name="QtInnerTestStructType"/>
 <xs:complexType name="ListOfQtInnerTestStructType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtInnerTestStructType" name="QtInnerTestStructType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtInnerTestStructType" name="ListOfQtInnerTestStructType" nillable="true"/>
 <xs:complexType name="QtStructWithOptionalFieldType">
  <xs:sequence>
   <xs:element minOccurs="0" type="xs:unsignedInt" name="EncodingMask"/>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="MandatoryMember"/>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:double" name="OptionalMember"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:QtStructWithOptionalFieldType" name="QtStructWithOptionalFieldType"/>
 <xs:complexType name="ListOfQtStructWithOptionalFieldType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtStructWithOptionalFieldType" name="QtStructWithOptionalFieldType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtStructWithOptionalFieldType" name="ListOfQtStructWithOptionalFieldType" nillable="true"/>
 <xs:complexType name="QtTestStructType">
  <xs:complexContent mixed="false">
   <xs:extension base="tns:QtAbstractBaseStruct">
    <xs:sequence>
     <xs:element minOccurs="0" maxOccurs="1" type="xs:string" name="StringMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:LocalizedText" name="LocalizedTextMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:QualifiedName" name="QualifiedNameMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:ListOfInt64" name="Int64ArrayMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="tns:QtTestEnumeration" name="EnumMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="tns:QtInnerTestStructType" name="NestedStructMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="tns:ListOfQtInnerTestStructType" name="NestedStructArrayMember"/>
    </xs:sequence>
   </xs:extension>
  </xs:complexContent>
 </xs:complexType>
 <xs:element type="tns:QtTestStructType" name="QtTestStructType"/>
 <xs:complexType name="ListOfQtTestStructType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtTestStructType" name="QtTestStructType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtTestStructType" name="ListOfQtTestStructType" nillable="true"/>
 <xs:complexType name="QtTestStructWithDataValue">
  <xs:complexContent mixed="false">
   <xs:extension base="tns:QtAbstractBaseStruct">
    <xs:sequence>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:DataValue" name="DataValueMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:Variant" name="VariantMember"/>
    </xs:sequence>
   </xs:extension>
  </xs:complexContent>
 </xs:complexType>
 <xs:element type="tns:QtTestStructWithDataValue" name="QtTestStructWithDataValue"/>
 <xs:complexType name="ListOfQtTestStructWithDataValue">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtTestStructWithDataValue" name="QtTestStructWithDataValue" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtTestStructWithDataValue" name="ListOfQtTestStructWithDataValue" nillable="true"/>
 <xs:complexType name="QtTestStructWithDiagnosticInfo">
  <xs:complexContent mixed="false">
   <xs:extension base="tns:QtAbstractBaseStruct">
    <xs:sequence>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:DiagnosticInfo" name="DiagnosticInfoMember"/>
     <xs:element minOccurs="0" maxOccurs="1" type="ua:ListOfDiagnosticInfo" name="DiagnosticInfoArrayMember"/>
    </xs:sequence>
   </xs:extension>
  </xs:complexContent>
 </xs:complexType>
 <xs:element type="tns:QtTestStructWithDiagnosticInfo" name="QtTestStructWithDiagnosticInfo"/>
 <xs:complexType name="ListOfQtTestStructWithDiagnosticInfo">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtTestStructWithDiagnosticInfo" name="QtTestStructWithDiagnosticInfo" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtTestStructWithDiagnosticInfo" name="ListOfQtTestStructWithDiagnosticInfo" nillable="true"/>
 <xs:complexType name="QtTestUnionType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="1" type="xs:unsignedInt" name="SwitchField"/>
   <xs:choice>
    <xs:element minOccurs="0" maxOccurs="1" type="xs:long" name="Member1"/>
    <xs:element minOccurs="0" maxOccurs="1" type="tns:QtInnerTestStructType" name="Member2"/>
   </xs:choice>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:QtTestUnionType" name="QtTestUnionType"/>
 <xs:complexType name="ListOfQtTestUnionType">
  <xs:sequence>
   <xs:element minOccurs="0" maxOccurs="unbounded" type="tns:QtTestUnionType" name="QtTestUnionType" nillable="true"/>
  </xs:sequence>
 </xs:complexType>
 <xs:element type="tns:ListOfQtTestUnionType" name="ListOfQtTestUnionType" nillable="true"/>
</xs:schema>
                           NamespaceUri����   NamespaceUri    ��������                     ����              
    ` h t t p : / / q t - p r o j e c t . o r g / Q t O p c U a T e s t M o d e l / T y p e s . x s d                      �    QtInnerTestStructType����   QtInnerTestStructType    ��������                     ����              
    * Q t I n n e r T e s t S t r u c t T y p e                      �    QtInnerTestStructType����   QtInnerTestStructType    ��������                     ����              
    V / / x s : e l e m e n t [ @ n a m e = ' Q t I n n e r T e s t S t r u c t T y p e ' ]                      �    QtStructWithOptionalFieldType����   QtStructWithOptionalFieldType    ��������                     ����              
    : Q t S t r u c t W i t h O p t i o n a l F i e l d T y p e                      �    QtStructWithOptionalFieldType����   QtStructWithOptionalFieldType    ��������                     ����              
    f / / x s : e l e m e n t [ @ n a m e = ' Q t S t r u c t W i t h O p t i o n a l F i e l d T y p e ' ]                      �    QtTestStructType����   QtTestStructType    ��������                     ����              
      Q t T e s t S t r u c t T y p e                      �    QtTestStructType����   QtTestStructType    ��������                     ����              
    L / / x s : e l e m e n t [ @ n a m e = ' Q t T e s t S t r u c t T y p e ' ]                      �    QtTestUnionType����   QtTestUnionType    ��������                     ����              
     Q t T e s t U n i o n T y p e                      �    QtTestUnionType����   QtTestUnionType    ��������                     ����              
    J / / x s : e l e m e n t [ @ n a m e = ' Q t T e s t U n i o n T y p e ' ]                      �    QtAbstractBaseStruct����   QtAbstractBaseStruct    ��������                     ����              
    ( Q t A b s t r a c t B a s e S t r u c t                      �    QtAbstractBaseStruct����   QtAbstractBaseStruct    ��������                     ����              
    T / / x s : e l e m e n t [ @ n a m e = ' Q t A b s t r a c t B a s e S t r u c t ' ]                      �    StructWithDataValue       StructWithDataValue����                           �����                    j       �       q         .                �         -       �       �         &       �       �         &       �       �         &       �       �         &       �       �         &       �       �         &        1�       �         -       �       �         &       �       �         &                �         -       �       �         &       �       �         &                �         -       �       �         -       �       �         -       �       �         -       �       �         -       �       �         -       �       �         &       �       �         &       �       �         &       �       �         &       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       t         '       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       �         '         U       �         #       �         =         (       �       y         /       �       r         /       �       �         #       �       v         /       �       z         /       �       {         /       �       s         /       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       �         '       �         L         (       �       u         '       �         L         (       �       w         '       �         L         (       �       x         '       q         N         %       q         D         (       r         ?         (       s         ?         (       t         E         (       u         E         (       v         ?         (       w         E         (       x         E         (       y         ?         (       z         ?         (       {         ?         (       |         H         (       |       }         .       |       �         /       |       �         /       |       �         /       |       �         /       |       w         /       |       t         /       |       �         /         ]       |         /       }         D         (       ~         H         (       ~                .       ~       �         /       ~       �         /       ~       �         /       ~       �         /       ~       x         /       ~       u         /       ~       �         /         \       ~         /                D         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         E         (       �         ?         (          q       �           t       |           u       ~           w       |           x       ~           }       |           }        �                 ~                    k          �       |           �       ~           �       |           �       ~           �       |           �       ~           �       |           �       ~           �       |           �       ~                                  basyskom_ua   basyskom_ua %�e�O)   