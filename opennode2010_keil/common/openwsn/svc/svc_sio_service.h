
intx sac_send( TiNioAcceptor * nac, TiFrame * frame, uint8 option );

/** Move the frame received inside rxque into frame object */
intx sio_service_recv( TiNioAcceptor * nac, TiFrame * frame ,uint8 option);
