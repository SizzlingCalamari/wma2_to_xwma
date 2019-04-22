#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <algorithm>
#include <list>
#include <vector>

#pragma pack(push, 1)

struct WaveFormatEx
{
    uint16_t wFormatTag;
    uint16_t nChannels;
    uint32_t nSamplesPerSec;
    uint32_t nAvgBytesPerSec;
    uint16_t nBlockAlign;
    uint16_t wBitsPerSample;
    uint16_t cbSize;
};

struct Wma2ExtraData
{
    uint32_t flags1;
    union
    {
        uint16_t flags2;
        struct
        {
            uint16_t exponentialVLCs : 1;
            uint16_t useBitReservoir : 1;
            uint16_t variableBlockLength : 1;
            uint16_t pad : 13;
        };
    };
    uint32_t unused;
};

namespace asf
{
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

    struct SpreadAudioErrorCorrectionData
    {
        uint8_t span;
        uint16_t virtualPacketLength;
        uint16_t virtualChunkLength;
        uint16_t silenceDataLength;
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

    union PayloadFlags
    {
        uint8_t flags;
        struct
        {
            uint8_t numberOfPayloads : 6;
            uint8_t payloadLengthType : 2;
        };
    };

    union PayloadStreamNumber
    {
        uint8_t streamNumber : 7;
        uint8_t keyFrameBit : 1;
    };
}

namespace riff
{
    struct RiffChunk
    {
        uint32_t id;
        uint32_t size;

        uint32_t format;
    };

    struct FmtChunk
    {
        uint32_t id;
        uint32_t size;

        WaveFormatEx format;
    };

    struct DataChunk
    {
        uint32_t id;
        uint32_t size;
    };

    struct DpdsChunk
    {
        uint32_t id;
        uint32_t size;
    };
}

#pragma pack(pop)

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

    const char* inputFile = argv[1];
    FILE* fp = fopen(inputFile, "rb");
    if (!fp)
    {
        return -1;
    }

    FILE* outFp = fopen(argv[2], "wb");
    if (!outFp)
    {
        fclose(fp);
        return -1;
    }

    WaveFormatEx waveFormat;
    Wma2ExtraData extraData;
    std::list<std::vector<uint8_t>> packets;

    asf::GUID guid;
    uint64_t size;
    bool hasErrorCorrection = false;

    while (fpeek(fp), !feof(fp))
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

                    asf::SpreadAudioErrorCorrectionData errorData;
                    assert(sizeof(waveFormat) <= stream.typeSpecificDataLength);
                    assert(sizeof(errorData) <= stream.errorCorrectionDataLength);

                    fread(&waveFormat, sizeof(waveFormat), 1, fp);
                    assert(waveFormat.wFormatTag == 0x161); // WMAv2
                    assert(sizeof(waveFormat) + waveFormat.cbSize == stream.typeSpecificDataLength);
                    assert(sizeof(extraData) == waveFormat.cbSize);
                    fread(&extraData, sizeof(extraData), 1, fp);

                    fread(&errorData, sizeof(errorData), 1, fp);
                    assert(sizeof(errorData) + errorData.silenceDataLength == stream.errorCorrectionDataLength);
                    fseek(fp, errorData.silenceDataLength, SEEK_CUR);
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
                        descriptorNameLength = std::min<uint16_t>(descriptorNameLength, sizeof(descriptorName));
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
                        codecNameLength = std::min<uint16_t>(codecNameLength, sizeof(codecName) / sizeof(char16_t));
                        fread(&codecName, sizeof(char16_t), codecNameLength, fp);
                        fread(&codecDescriptionLength, sizeof(codecDescriptionLength), 1, fp);
                        codecDescriptionLength = std::min<uint16_t>(codecDescriptionLength, sizeof(codecDescription) / sizeof(char16_t));
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

                if (flags.errorCorrectionPresent)
                {
                    assert(flags.errorCorrectionDataLength == 2);
                    assert(!flags.opaqueDataPresent);
                    assert(flags.errorCorrectionLengthType == 0);

                    fgetc(fp);
                    asf::ErrorCorrectionData data;
                    fread(&data, sizeof(asf::ErrorCorrectionData), 1, fp);
                    assert((data.type == 0) && (data.cycle == 0) && !hasErrorCorrection);
                }

                asf::PayloadParsingInformation::LengthTypeFlags lengthTypeFlags;
                asf::PayloadParsingInformation::PropertyFlags propertyFlags;

                fread(&lengthTypeFlags, sizeof(lengthTypeFlags), 1, fp);
                fread(&propertyFlags, sizeof(propertyFlags), 1, fp);

                assert(lengthTypeFlags.sequenceType == 0);
                assert(lengthTypeFlags.packetLengthType == 0);
                assert(!lengthTypeFlags.errorCorrectionPresent);

                assert(propertyFlags.replicatedDataLengthType == 1);
                assert(propertyFlags.offsetIntoMediaObjectLengthType == 3);
                assert(propertyFlags.mediaObjectNumberLengthType == 1);
                assert(propertyFlags.streamNumberLengthType == 1);

                uint8_t paddingLength8;
                uint16_t paddingLength16;
                uint32_t paddingLength32;

                uint32_t sendTime;
                uint16_t duration;
                switch(lengthTypeFlags.paddingLengthType)
                {
                case 0:
                default:
                    break;
                case 1:
                    fread(&paddingLength8, sizeof(paddingLength8), 1, fp);
                    break;
                case 2:
                    fread(&paddingLength16, sizeof(paddingLength16), 1, fp);
                    break;
                case 3:
                    fread(&paddingLength32, sizeof(paddingLength32), 1, fp);
                    break;
                }
                fread(&sendTime, sizeof(sendTime), 1, fp);
                fread(&duration, sizeof(duration), 1, fp);

                assert(lengthTypeFlags.multiplePayloadsPresent);

                asf::PayloadFlags payloadFlags;
                fread(&payloadFlags, sizeof(payloadFlags), 1, fp);
                assert(payloadFlags.numberOfPayloads > 0);
                assert(payloadFlags.payloadLengthType == 2);

                for (uint8_t j = 0; j < payloadFlags.numberOfPayloads; ++j)
                {
                    asf::PayloadStreamNumber streamNumber;
                    fread(&streamNumber, sizeof(streamNumber), 1, fp);

                    uint8_t mediaObjectNumber;
                    uint32_t offsetIntoMediaObject;
                    uint8_t replicatedDataLength;
                    uint16_t payloadLength;

                    fread(&mediaObjectNumber, sizeof(mediaObjectNumber), 1, fp);
                    fread(&offsetIntoMediaObject, sizeof(offsetIntoMediaObject), 1, fp);
                    fread(&replicatedDataLength, sizeof(replicatedDataLength), 1, fp);
                    assert(replicatedDataLength != 1);
                    assert(replicatedDataLength == 0 || replicatedDataLength >= 8);
                    fseek(fp, replicatedDataLength, SEEK_CUR);
                    fread(&payloadLength, sizeof(payloadLength), 1, fp);
                    assert(payloadLength > 0);

                    std::vector<uint8_t> payload(payloadLength);
                    fread(payload.data(), 1, payloadLength, fp);

                    packets.emplace_back(std::move(payload));
                }

                switch (lengthTypeFlags.paddingLengthType)
                {
                case 0:
                default:
                    break;
                case 1:
                    fseek(fp, paddingLength8, SEEK_CUR);
                    break;
                case 2:
                    fseek(fp, paddingLength16, SEEK_CUR);
                    break;
                case 3:
                    fseek(fp, paddingLength32, SEEK_CUR);
                    break;
                }
            }
        }
        else
        {
            fseek(fp, size, SEEK_CUR);
        }
    }
    fclose(fp);

    {
        riff::RiffChunk riff;
        riff::FmtChunk fmt;
        riff::DpdsChunk dpds;
        riff::DataChunk data;

        riff.id = 'FFIR';
        riff.size = 0;
        riff.format = 'AMWX';

        fmt.id = ' tmf';
        fmt.size = sizeof(WaveFormatEx) + waveFormat.cbSize;
        fmt.format = waveFormat;
        //fmt.format.cbSize = 0;

        dpds.id = 'sdpd';
        dpds.size = packets.size() * sizeof(uint32_t);

        fwrite(&riff, sizeof(riff), 1, outFp);
        fwrite(&fmt, sizeof(fmt), 1, outFp);
        fwrite(&extraData, sizeof(extraData), 1, outFp);
        fwrite(&dpds, sizeof(dpds), 1, outFp);

        const uint32_t samplesPerPacket = 2048;
        const uint32_t uncompressedSamples = samplesPerPacket * packets.size();

        uint32_t dpdsOffset = 0;
        uint32_t compressedSize = 0;
        for (const std::vector<uint8_t>& packet : packets)
        {
            compressedSize += packet.size();

            dpdsOffset += (samplesPerPacket * fmt.format.wBitsPerSample / 8);
            fwrite(&dpdsOffset, sizeof(dpdsOffset), 1, outFp);
        }

        const uint32_t bytesPerSample = fmt.format.wBitsPerSample / 8;
        const uint32_t duration = dpdsOffset / bytesPerSample;

        assert(uncompressedSamples == duration);

        data.id = 'atad';
        data.size = compressedSize;
        fwrite(&data, sizeof(data), 1, outFp);

        for (const std::vector<uint8_t>& packet : packets)
        {
            fwrite(packet.data(), 1, packet.size(), outFp);
        }

        uint32_t riffSize = ftell(outFp) - 8;
        fseek(outFp, 4, SEEK_SET);
        fwrite(&riffSize, sizeof(riffSize), 1, outFp);
    }
    fclose(outFp);
    return 0;
}
