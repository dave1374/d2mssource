using D2MSCommon.DataStructures;
using System;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceModel.Web;


namespace D2MSWebApp
{
    [ServiceContract]
    public interface IService1
    {
        //remove  UriTemplate = "/SaveData")] for ssl support
        [OperationContract]
        [WebInvoke(
            Method         = "POST",
            BodyStyle      = WebMessageBodyStyle.WrappedRequest,
            RequestFormat  = WebMessageFormat.Json,
            ResponseFormat = WebMessageFormat.Json
            , UriTemplate = "/SaveData")]
        String SaveData(D2MSData data);

        [OperationContract]
        [WebInvoke(
            Method = "GET",
            BodyStyle = WebMessageBodyStyle.WrappedRequest,
            RequestFormat = WebMessageFormat.Json,
            ResponseFormat = WebMessageFormat.Json
            , UriTemplate = "/GetGenericItems")]
        List<GenericItemDisplay> GetGenericItems();
        
        [OperationContract]
        [WebInvoke(
            Method = "GET",
            BodyStyle = WebMessageBodyStyle.WrappedRequest,
            RequestFormat = WebMessageFormat.Json,
            ResponseFormat = WebMessageFormat.Json
            , UriTemplate = "/GetLatestVersion")]
        string GetLatestVersion();


        [OperationContract]
        [WebInvoke(
            Method = "GET",
            BodyStyle = WebMessageBodyStyle.WrappedRequest,
            RequestFormat = WebMessageFormat.Json,
            ResponseFormat = WebMessageFormat.Json
            , UriTemplate = "/GetLevels")]
        List<LevelDisplay> GetLevels();


    }
}
