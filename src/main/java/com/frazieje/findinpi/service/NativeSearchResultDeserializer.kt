package com.frazieje.findinpi.service

import com.google.gson.JsonDeserializationContext
import com.google.gson.JsonDeserializer
import com.google.gson.JsonElement
import java.lang.reflect.Type

class NativeSearchResultDeserializer : JsonDeserializer<NativeSearchResult> {
    override fun deserialize(
        json: JsonElement, typeOfT: Type, context: JsonDeserializationContext
    ): NativeSearchResult = NativeSearchResult(
        json.asJsonObject.get("results").asJsonArray.get(0).asJsonObject.get("offset").asLong,
            json.asJsonObject.get("results").asJsonArray.get(0).asJsonObject.get("excerpt").asString,
        json.asJsonObject.get("results").asJsonArray.get(0).asJsonObject.get("excerptOffset").asInt
        )
}