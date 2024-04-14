package com.frazieje.findinpi.service;

import com.frazieje.findinpi.model.SearchResult;
import org.jetbrains.annotations.NotNull;

public class NativePiFinder implements PiFinder {

    static {
        System.loadLibrary("bigfind");
    }

    @Override
    public native @NotNull SearchResult search(@NotNull String dataFilePath, @NotNull String searchText, int bufferSize);
}
