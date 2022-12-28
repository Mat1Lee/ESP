void streamCallback(MultiPathStreamData stream)
{
  size_t numChild = sizeof(childPath) / sizeof(childPath[0]);

  for (size_t i = 0; i < numChild; i++)
  {
    if (stream.get(childPath[i]))
    {
      if (i == 0)
      {
        SoDien_CB = ((String)stream.value.c_str()).toFloat();
        Serial.println(SoDien_CB);
      }
      else if (i == 1)
      {
        String data = "";
        data = stream.value.c_str();
        Serial.println(data);
        if (data == "true")
        {
          sts_getData = true;
        }
        else if (data == "false")
        {
          sts_getData = false;
        }
      }
    }
  }
  //  Serial.println();
}

void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}
