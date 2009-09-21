namespace test.sacs
{
    /// <summary>Test DataWriter Lookup.</summary>
    /// <remarks>Test DataWriter Lookup.</remarks>
    public class Publisher8 : Test.Framework.TestCase
    {
        /// <summary>Test DataWriter Lookup.</summary>
        /// <remarks>Test DataWriter Lookup.</remarks>
        public Publisher8()
            : base("sacs_publisher_tc8", "sacs_publisher", "publisher", "Check if copy_from_topic_qos rejects TOPIC_QOS_DEFAULT with correct code."
                , "Check if copy_from_topic_qos rejects TOPIC_QOS_DEFAULT with correct code.", null
                )
        {
            this.AddPreItem(new test.sacs.PublisherItemInit());
            this.AddPreItem(new test.sacs.TopicInit());
            this.AddPostItem(new test.sacs.TopicDeinit());
            this.AddPostItem(new test.sacs.PublisherItemDeinit());
        }

        public override Test.Framework.TestResult Run()
        {
            DDS.IPublisher publisher;
            DDS.DataWriterQos dataWriterQos;
            DDS.DataWriterQos qosHolder1;
            DDS.DataWriterQos qosHolder2;
            DDS.IDataWriter writer;
            DDS.ITopic topic;
            string expResult = "copy_from_topic_qos rejects TOPIC_QOS_DEFAULT with correct code.";
            Test.Framework.TestResult result;
            DDS.ReturnCode rc = DDS.ReturnCode.Error;
            result = new Test.Framework.TestResult(expResult, string.Empty, Test.Framework.TestVerdict.Pass,
                Test.Framework.TestVerdict.Fail);
            publisher = (DDS.IPublisher)this.ResolveObject("publisher");
            topic = (DDS.ITopic)this.ResolveObject("topic");

            if (publisher.GetDefaultDataWriterQos(out qosHolder1) != DDS.ReturnCode.Ok)
            {
                result.Result = "Could not retrieve default DataWriterQos";
                return result;
            }
            dataWriterQos = qosHolder1;
            dataWriterQos.History.Kind = DDS.HistoryQosPolicyKind.KeepAllHistoryQos;
            dataWriterQos.History.Depth = 150;


            // TODO: JLS, DDS.TopicQos.Default does not exist
            //rc = publisher.CopyFromTopicQos(out qosHolder1, ref DDS.TopicQos.Default);
            if (rc != DDS.ReturnCode.BadParameter)
            {
                result.Result = "copy_from_topic_qos retuns wrong code (RETCODE = " + rc + ").";
                return result;
            }
            result.Result = expResult;
            result.Verdict = Test.Framework.TestVerdict.Pass;
            return result;
        }
    }
}