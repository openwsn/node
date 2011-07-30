intx sac_send( TiNioAcceptor * nac, TiFrame * frame, uint8 option );

/** Move the frame received inside rxque into frame object */
intx sac_recv( TiNioAcceptor * nac, TiFrame * frame ,uint8 option);
