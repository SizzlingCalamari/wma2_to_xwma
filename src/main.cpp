#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

namespace asf
{
#pragma pack(push, 1)

    struct GUID
    {
        uint32_t a;
        uint16_t b;
        uint16_t c;
        uint8_t d[8];
    };

    // Top-level ASF object GUIDs
    constexpr const GUID ASF_Header_Object = {
        0x75B22630, 0x668E, 0x11CF, { 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C }
    };
    constexpr const GUID ASF_Data_Object = {
        0x75B22636, 0x668E, 0x11CF, { 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C }
    };

    // Header Object GUIDs
    constexpr const GUID ASF_File_Properties_Object = {
        0x8CABDCA1, 0xA947, 0x11CF, { 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }
    };
    constexpr const GUID ASF_Stream_Properties_Object = {
        0xB7DC0791, 0xA9B7, 0x11CF, { 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }
    };
    constexpr const GUID ASF_Header_Extension_Object = {
        0x5FBF03B5, 0xA92E, 0x11CF, { 0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }
    };
    constexpr const GUID ASF_Codec_List_Object = {
        0x86D15240, 0x311D, 0x11D0, { 0xA3, 0xA4, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6 }
    };
    constexpr const GUID ASF_Error_Correction_Object = {
        0x75B22635, 0x668E, 0x11CF, { 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C }
    };
    constexpr const GUID ASF_Extended_Content_Description_Object = {
        0xD2D0A440, 0xE307, 0x11D2, { 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50 }
    };

    // Stream Properties Object Stream Type GUIDs
    constexpr const GUID ASF_Audio_Media = {
        0xF8699E40, 0x5B4D, 0x11CF, { 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B }
    };

    // Stream Properties Object Error Correction Type GUIDs
    constexpr const GUID ASF_No_Error_Correction = {
        0x20FB5700, 0x5B55, 0x11CF, { 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B }
    };
    constexpr const GUID ASF_Audio_Spread = {
        0xBFC3CD50, 0x618F, 0x11CF, { 0x8B, 0xB2, 0x00, 0xAA, 0x00, 0xB4, 0xE2, 0x20 }
    };

    // Header Extension Object GUIDs
    constexpr const GUID ASF_Reserved_1 = {
        0xABD3D211, 0xA9BA, 0x11CF, { 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65 }
    };

    // Codec List Object GUIDs
    constexpr const GUID ASF_Reserved_2 = {
        0x86D15241, 0x311D, 0x11D0, { 0xA3, 0xA4, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6 }
    };

    struct HeaderObject
    {
        uint32_t numObjects;
        uint8_t reserved1; // Must be 0x1
        uint8_t reserved2; // Must be 0x2
    };

    struct FilePropertiesObject
    {
        GUID fileId;
        uint64_t fileSize;
        uint64_t creationDate;
        uint64_t dataPacketsCount;
        uint64_t playDuration;
        uint64_t sendDuration;
        uint64_t preroll;
        union Flags
        {
            uint32_t flags;
            struct
            {
                uint32_t broadcastFlag : 1;
                uint32_t seekableFlag : 1;
                uint32_t reserved : 30;
            };
        } flags;
        uint32_t minDataPacketSize;
        uint32_t maxDataPacketSize;
        uint32_t maxBitrate;
    };

    struct StreamPropertiesObject
    {
        GUID streamType;
        GUID errorCorrectionType;
        uint64_t timeOffset;
        uint32_t typeSpecificDataLength;
        uint32_t errorCorrectionDataLength;
        union Flags
        {
            uint16_t flags;
            struct
            {
                uint16_t streamNumber : 7;
                uint16_t reserved : 8;
                uint16_t encryptedContentFlag : 1;
            };
        } flags;
        uint32_t reserved;
    };

    struct HeaderExtensionObject
    {
        GUID reserved1; // Must be ASF_Reserved_1
        uint16_t reserved2; // Must be 6
        uint32_t headerExtensionDataSize;
    };

    struct DataObject
    {
        GUID fileId;
        uint64_t totalDataPackets;
        uint16_t reserved; // Must be 0x101
    };

    union ErrorCorrectionFlags
    {
        uint8_t flags;
        struct
        {
            uint8_t errorCorrectionDataLength : 4;
            uint8_t opaqueDataPresent : 1;
            uint8_t errorCorrectionLengthType : 2;
            uint8_t errorCorrectionPresent : 1;
        };
    };

    struct ErrorCorrectionData
    {
        union
        {
            uint8_t typeval;
            struct
            {
                uint8_t type : 4;
                uint8_t number : 4;
            };
        };
        uint8_t cycle;
    };

    namespace PayloadParsingInformation
    {
        union LengthTypeFlags
        {
            uint8_t flags;
            struct
            {
                uint8_t multiplePayloadsPresent : 1;
                uint8_t sequenceType : 2;
                uint8_t paddingLengthType : 2;
                uint8_t packetLengthType : 2;
                uint8_t errorCorrectionPresent : 1;
            };
        };

        union PropertyFlags
        {
            uint8_t flags;
            struct
            {
                uint8_t replicatedDataLengthType : 2;
                uint8_t offsetIntoMediaObjectLengthType : 2;
                uint8_t mediaObjectNumberLengthType : 2;
                uint8_t streamNumberLengthType : 2;
            };
        };
    }

#pragma pack(pop)
}

static int fpeek(FILE* fp)
{
    const int c = getc(fp);
    ungetc(c, fp);
    return c;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        return -1;
    }

    const char* file = argv[1];
    FILE* fp = fopen(file, "rb");
    if (!fp)
    {
        return -1;
    }

    asf::GUID guid;
    uint64_t size;
    bool hasErrorCorrection = false;

    while (!feof(fp))
    {
        fread(&guid, sizeof(guid), 1, fp);
        fread(&size, sizeof(size), 1, fp);
        assert(size >= 24);
        size -= 24;
        if (!memcmp(&guid, &asf::ASF_Header_Object, sizeof(guid)))
        {
            asf::HeaderObject header;
            fread(&header, sizeof(header), 1, fp);
            assert(header.reserved1 == 0x1);
            assert(header.reserved2 == 0x2);

            for (uint32_t i = 0; i < header.numObjects; ++i)
            {
                fread(&guid, sizeof(guid), 1, fp);
                fread(&size, sizeof(size), 1, fp);
                assert(size >= 24);
                size -= 24;
                if (!memcmp(&guid, &asf::ASF_File_Properties_Object, sizeof(guid)))
                {
                    asf::FilePropertiesObject file;
                    fread(&file, sizeof(file), 1, fp);
                    assert(!file.flags.broadcastFlag);
                    assert(file.flags.reserved == 0);
                    assert(file.minDataPacketSize == file.maxDataPacketSize);
                }
                else if (!memcmp(&guid, &asf::ASF_Stream_Properties_Object, sizeof(guid)))
                {
                    asf::StreamPropertiesObject stream;
                    fread(&stream, sizeof(stream), 1, fp);
                    assert(!memcmp(&stream.streamType, &asf::ASF_Audio_Media, sizeof(asf::ASF_Audio_Media)));
                    assert(stream.flags.reserved == 0);
                    assert(stream.flags.encryptedContentFlag == 0);
                    assert(stream.reserved == 0);
                    fseek(fp, stream.typeSpecificDataLength + stream.errorCorrectionDataLength, SEEK_CUR);
                }
                else if (!memcmp(&guid, &asf::ASF_Header_Extension_Object, sizeof(guid)))
                {
                    asf::HeaderExtensionObject ext;
                    fread(&ext, sizeof(ext), 1, fp);
                    assert(!memcmp(&ext.reserved1, &asf::ASF_Reserved_1, sizeof(asf::ASF_Reserved_1)));
                    assert(ext.reserved2 == 6);
                    assert(ext.headerExtensionDataSize == 0);
                }
                else if (!memcmp(&guid, &asf::ASF_Extended_Content_Description_Object, sizeof(guid)))
                {
                    uint16_t contentDescriptorsCount;
                    fread(&contentDescriptorsCount, sizeof(contentDescriptorsCount), 1, fp);
                    for (uint16_t j = 0; j < contentDescriptorsCount; ++j)
                    {
                        uint16_t descriptorNameLength;
                        uint16_t descriptorValueDataType;
                        uint16_t descriptorValueLength;
                        char16_t descriptorName[260];

                        fread(&descriptorNameLength, sizeof(descriptorNameLength), 1, fp);
                        fread(&descriptorName, 1, descriptorNameLength, fp);
                        fread(&descriptorValueDataType, sizeof(descriptorValueDataType), 1, fp);
                        fread(&descriptorValueLength, sizeof(descriptorValueLength), 1, fp);
                        fseek(fp, descriptorValueLength, SEEK_CUR);
                    }
                }
                else if (!memcmp(&guid, &asf::ASF_Codec_List_Object, sizeof(guid)))
                {
                    asf::GUID reserved;
                    uint32_t codecEntriesCount;

                    fread(&reserved, sizeof(reserved), 1, fp);
                    fread(&codecEntriesCount, sizeof(codecEntriesCount), 1, fp);
                    assert(!memcmp(&reserved, &asf::ASF_Reserved_2, sizeof(asf::ASF_Reserved_2)));
                    for (uint32_t j = 0; j < codecEntriesCount; ++j)
                    {
                        uint16_t type;
                        uint16_t codecNameLength;
                        char16_t codecName[260];
                        uint16_t codecDescriptionLength;
                        char16_t codecDescription[260];
                        uint16_t codecInformationLength;

                        fread(&type, sizeof(type), 1, fp);
                        fread(&codecNameLength, sizeof(codecNameLength), 1, fp);
                        fread(&codecName, sizeof(char16_t), codecNameLength, fp);
                        fread(&codecDescriptionLength, sizeof(codecDescriptionLength), 1, fp);
                        fread(&codecDescription, sizeof(char16_t), codecDescriptionLength, fp);
                        fread(&codecInformationLength, sizeof(codecInformationLength), 1, fp);
                        fseek(fp, codecInformationLength, SEEK_CUR);

                        assert(type == 2);
                    }
                }
                else
                {
                    fseek(fp, size, SEEK_CUR);
                }

                if (!hasErrorCorrection && !memcmp(&guid, &asf::ASF_Error_Correction_Object, sizeof(guid)))
                {
                    hasErrorCorrection = true;
                }
            }
        }
        else if (!memcmp(&guid, &asf::ASF_Data_Object, sizeof(guid)))
        {
            asf::DataObject dataObject;
            fread(&dataObject, sizeof(dataObject), 1, fp);
            assert(dataObject.reserved == 0x101);
            for (uint64_t i = 0; i < dataObject.totalDataPackets; ++i)
            {
                asf::ErrorCorrectionFlags flags;
                flags.flags = static_cast<uint8_t>(fpeek(fp));

                assert(flags.errorCorrectionDataLength == 2);
                assert(!flags.opaqueDataPresent);
                assert(flags.errorCorrectionLengthType == 0);

                if (flags.errorCorrectionPresent)
                {
                    fgetc(fp);
                    asf::ErrorCorrectionData data;
                    fread(&data, sizeof(asf::ErrorCorrectionData), 1, fp);
                    assert((data.type == 0) && !hasErrorCorrection);
                }

                asf::PayloadParsingInformation::LengthTypeFlags lengthTypeFlags;
                asf::PayloadParsingInformation::PropertyFlags propertyFlags;

                fread(&lengthTypeFlags, sizeof(lengthTypeFlags), 1, fp);
                fread(&propertyFlags, sizeof(propertyFlags), 1, fp);

                //assert(!lengthTypeFlags.multiplePayloadsPresent);
                assert(lengthTypeFlags.sequenceType == 0);

            }
        }
        else
        {
            fseek(fp, size, SEEK_CUR);
        }
    }
    return 0;
}
