package com.frazieje.findinpi.service;

import com.frazieje.findinpi.model.SearchResult;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import org.jetbrains.annotations.NotNull;

import java.util.ArrayList;

public class NativePiFinder implements PiFinder {

    static {
        System.loadLibrary("bigfind");
    }

    private final Gson gson = new GsonBuilder().registerTypeAdapter(FemtoSearchResult.class, new FemtoSearchResultDeserializer()).registerTypeAdapter(FemtoCountResult.class, new FemtoCountResultDeserializer()).create();

    @Override
    public native void init(@NotNull String dataFilePath);

    private native NativeResult searchInternal(String searchText, int maxResultCount);

    private native NativeResult countInternal(String searchText);

    @Override
    public @NotNull SearchResult search(@NotNull String searchText, int maxResultCount) {
        var nativeResult = countInternal(searchText);
        var countTime = nativeResult.getSearchTimeMs();
        var countResult = gson.fromJson(nativeResult.getFemtoResultJson(), FemtoCountResult.class);
        nativeResult = searchInternal(searchText, maxResultCount);
        var searchResult = gson.fromJson(nativeResult.getFemtoResultJson(), FemtoSearchResult.class);
        return new SearchResult(
                countResult.getCount(),
                searchResult.getOffsets(),
                countTime + nativeResult.getSearchTimeMs(),
                null
        );
    }
}
