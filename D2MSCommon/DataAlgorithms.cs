using D2MSCommon.DataStructures;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace D2MSCommon
{
    public static class DataAlgorithms
    {

        public static ItemStructure DecompressValue(ulong x)
        {
            char c0 = Convert.ToChar(x >> 56 & 0xFF);
            char c1 = Convert.ToChar(x >> 48 & 0xFF);
            char c2 = Convert.ToChar(x >> 40 & 0xFF);

            byte lvlNo   = Convert.ToByte(x >> 32 & 0xFF);
            byte quality = Convert.ToByte(x >> 28 & 0xF);

            return new ItemStructure()
            {
                ItemCode = string.Join("", c0, c1, c2),
                LvlNo    = lvlNo,
                ItemQuality = quality
            };
        }

    }
}
