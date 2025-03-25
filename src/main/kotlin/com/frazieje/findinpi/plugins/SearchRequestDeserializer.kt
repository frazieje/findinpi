package com.frazieje.findinpi.plugins

import com.frazieje.findinpi.model.SearchRequest
import com.google.gson.JsonDeserializationContext
import com.google.gson.JsonDeserializer
import com.google.gson.JsonElement
import java.lang.reflect.Type

class SearchRequestDeserializer : JsonDeserializer<SearchRequest> {
    override fun deserialize(
        json: JsonElement,
        typeOfT: Type,
        context: JsonDeserializationContext
    ): SearchRequest {
        val searchRequestObj = json.asJsonObject
        return SearchRequest(
            searchRequestObj.get(SearchRequest::searchText.name).asString,
            if (searchRequestObj.has(SearchRequest::maxResultCount.name)) {
                searchRequestObj.get(SearchRequest::maxResultCount.name).asInt
            } else 1
        )
    }
}