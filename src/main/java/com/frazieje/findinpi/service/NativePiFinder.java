package com.frazieje.findinpi.service;

import com.frazieje.findinpi.model.SearchResult;
import kotlin.jvm.functions.Function0;
import org.jetbrains.annotations.NotNull;

public class NativePiFinder implements PiFinder {

    static {
        System.loadLibrary("bigfind");
    }

    @NotNull
    @Override
    public native SearchResult search(
            @NotNull String dataFilePath,
            @NotNull String searchText,
            long bufferSize,
            long offset,
            long length,
            @NotNull Function0<Boolean> isActive
    );
}
