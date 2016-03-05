using D2MSCommon;
using D2MSCommon.DataStructures;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;

namespace D2MSWebApp
{
    public class Service1 : IService1
    {
        public List<GenericItemDisplay> GetGenericItems()
        {
            D2MSEntities db = new D2MSEntities();

            var genericItems = from i in db.GenericItems
                               select new GenericItemDisplay(){
                                   ItemCode = i.ItemCode,
                                   ItemName = i.ItemName,
                                   ItemQuality = i.Quality,
                                   ItemShortName = i.ItemShortName
                               };

            return genericItems.ToList();
        }

        public List<LevelDisplay> GetLevels()
        {

            D2MSEntities db = new D2MSEntities();

            var levels = from l in db.Levels
                               select new LevelDisplay()
                               {
                                   LvlNo = l.LvlNo,
                                   LvlName = l.LvlName
                               };

            return levels.ToList();
        }

        public string GetLatestVersion()
        {
            return "beta 1.1";
        }

        public string SaveData(D2MSData data)
        {
            D2MSEntities db = new D2MSEntities();

            Account a = db.Accounts.SingleOrDefault(x => x.AccountName == data.AccountName);
            Char c = null;

            if (a == null)
            {
                a = new Account() { AccountName = data.AccountName, AccountID = Guid.NewGuid() };

                db.Accounts.Add(a);
            }

            c = a.Chars.SingleOrDefault(x => x.CharName == data.CharName);

            if (c == null)
            {
                c = new Char() { CharName = data.CharName };
                a.Chars.Add(c);
            }


            DataPackage dataPackage = new DataPackage() { GameDifficulty = data.GameDiff };

            dataPackage.CreatedOn = DateTime.Now;


            for (int i = 0; i < data.CompressedData.Count(); i++)
            {
                //uint item = data.CompressedData[i].ItemKey; key is useless now

                ItemStructure value = DataAlgorithms.DecompressValue(data.CompressedData[i].ItemValue);


                ParsedItem pi = new ParsedItem()
                {
                    CompressedData = (long)data.CompressedData[i].ItemValue,
                    ItemCode = value.ItemCode,
                    Quality = value.ItemQuality,
                    LvlNo = value.LvlNo
                };

                dataPackage.ParsedItems.Add(pi);
            }

            for (int i = 0; i < data.Timers.Count(); i++)
            {
                TimeInRoom time = new TimeInRoom() { 
                    SecondsInRoom = data.Timers[i].Time,
                    LvlNo = (short)data.Timers[i].LvlNo
                };

                dataPackage.TimeInRooms.Add(time);
            }


            c.DataPackages.Add(dataPackage);

            db.SaveChanges();

            return "CompressedData.Count : " + data.CompressedData.Count();
        }


    //public ItemStructure DecompressKey(ulong x)
    //{
    //    char c0 = Convert.ToChar(x >> 56 & 0xFF);
    //    char c1 = Convert.ToChar(x >> 48 & 0xFF);
    //    char c2 = Convert.ToChar(x >> 40 & 0xFF);
    //    byte lvlNo = Convert.ToByte(x >> 32 & 0xFF);

    //    ushort smallX = Convert.ToUInt16(x >> 16 & 0xFFFF);
    //    ushort smallY = Convert.ToUInt16(x >> 0 & 0xFFFF);

    //    return new ItemStructure()
    //    {
    //        ItemCode = string.Join("", c0, c1, c2),
    //        LvlNo = lvlNo,
    //        SmallX = smallX,
    //        SmallY = smallY
    //    };
    //}
}

}
