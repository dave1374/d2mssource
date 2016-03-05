using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace D2MSCommon.DataStructures
{

    public class LevelDisplay
    {
        public int LvlNo { get; set; }
        public string LvlName { get; set; }
    }

    public class ItemStructure
    {
        public string ItemCode { get; set; }
        public byte LvlNo { get; set; }
        public byte ItemQuality { get; set; }
    }


    public class GenericItemDisplay
    {
        [DataMember]
        public string ItemCode { get; set; }

        [DataMember]
        public string ItemName { get; set; }

        [DataMember]
        public string ItemShortName { get; set; }
        
        [DataMember]
        public short ItemQuality { get; set; }
    }

    public class D2MSData
    {
        [DataMember]
        public string AccountName { get; set; }

        [DataMember]
        public string CharName { get; set; }

        [DataMember]
        public byte GameDiff { get; set; }

        [DataMember]
        public CompressedItemData[] CompressedData { get; set; }

        [DataMember]
        public TimeInRoomStruct[] Timers { get; set; }

    }



    public class TimeInRoomStruct
    {
        [DataMember]
        public int LvlNo { get; set; }

        [DataMember]
        public int Time { get; set; }
    }

    public class CompressedItemData
    {
        [DataMember]
        public uint ItemKey { get; set; }

        [DataMember]
        public ulong ItemValue { get; set; }
    }
}
