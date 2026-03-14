package com.frazieje.findinpi.service;

import com.frazieje.findinpi.model.SearchResult;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import org.jetbrains.annotations.NotNull;

import java.util.Collections;

public class NativePiFinder implements PiFinder {

    static {
        System.loadLibrary("bigfind");
    }

    private final Gson gson = new GsonBuilder().registerTypeAdapter(NativeSearchResult.class, new NativeSearchResultDeserializer()).registerTypeAdapter(NativeCountResult.class, new NativeCountResultDeserializer()).create();

    @Override
    public native void init(@NotNull String dataFilePath, @NotNull String suffixArrayFilePath, @NotNull String fmIndexFilePath);

    private native NativeResult searchInternal(String searchText);

    private native NativeResult countInternal(String searchText);

    @Override
    public @NotNull SearchResult search(@NotNull String searchText) {
        var nativeResult = countInternal(searchText);
        var countTime = nativeResult.getSearchTimeMs();
        var countResult = gson.fromJson(nativeResult.getResultJson(), NativeCountResult.class);
        if (countResult.getCount() > 0) {
            nativeResult = searchInternal(searchText);
            var searchResult = gson.fromJson(nativeResult.getResultJson(), NativeSearchResult.class);
            return new SearchResult(
                countResult.getCount(),
                searchResult.getOffsets(),
                countTime + nativeResult.getSearchTimeMs(),
                null
            );
        } else {
            return new SearchResult(
            0,
                Collections.emptyList(),
                countTime,
                null
            );
        }
    }
}
