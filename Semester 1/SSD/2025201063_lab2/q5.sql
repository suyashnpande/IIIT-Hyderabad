DELIMITER //
CREATE PROCEDURE PrintAllSubscribersWatchHistory()
BEGIN
    DECLARE done INT DEFAULT FALSE;
    DECLARE v_id INT;

    DECLARE watchHistrycur CURSOR FOR 
        SELECT SubscriberID FROM Subscribers;

    DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

    OPEN watchHistrycur;
    read_loop: LOOP
        FETCH watchHistrycur INTO v_id;
        IF done THEN
            LEAVE read_loop;
        END IF;

        IF EXISTS (SELECT 1 FROM WatchHistory WHERE SubscriberID = v_id) THEN
            SELECT CONCAT('Watch history for Subscriber ID ', v_id) AS Header;
            CALL GetWatchHistoryBySubscriber(v_id);
        END IF;
    END LOOP;
    CLOSE watchHistrycur;
END//

DELIMITER ;

-- call PrintAllSubscribersWatchHistory;
