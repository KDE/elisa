package org.kde.elisa;

import android.content.Context;
import android.content.Intent;

import org.qtproject.qt5.android.bindings.QtService;

/**
 * Created by mgallien on 02/05/17.
 */

public class ElisaService extends QtService {
    public static void startMyService(Context ctx) {
        ctx.startService(new Intent(ctx, ElisaService.class));
    }
}
