package com.frazieje.findinpi.service;

import com.frazieje.findinpi.model.SearchResult;
import kotlin.jvm.functions.Function0;
import org.jetbrains.annotations.NotNull;

public class NativePiFinder implements PiFinder {

    static {
        System.loadLibrary("bigfind");
    }

    @Override
    public native void init(@NotNull String dataFilePath, long readBufferSize);

    @Override
    public native @NotNull SearchResult search(@NotNull String searchText);
}
