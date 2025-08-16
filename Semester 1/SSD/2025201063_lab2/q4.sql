DELIMITER \\

CREATE PROCEDURE SendWatchTimeReport()
BEGIN
    DECLARE done INT DEFAULT FALSE;
    DECLARE w_id INT;
    
    DECLARE watchCursor CURSOR FOR 
        SELECT DISTINCT SubscriberID FROM WatchHistory;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

    OPEN watchCursor;
    read_loop: LOOP
        FETCH watchCursor INTO w_id;
        IF done THEN
            LEAVE read_loop;
        END IF;
        CALL GetWatchHistoryBySubscriber(w_id);
    END LOOP;
    CLOSE watchCursor;
END\\
DELIMITER ;

-- call SendWatchTimeReport;