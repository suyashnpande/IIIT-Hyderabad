DELIMITER \\
CREATE PROCEDURE AddSubscriberIfNotExists(IN subName VARCHAR(100))
BEGIN
    DECLARE cnt INT;
    DECLARE newID INT;
    
    SELECT COUNT(*) INTO cnt 
    FROM Subscribers 
    WHERE SubscriberName = subName;

    IF cnt = 0 THEN
        SELECT IFNULL(MAX(SubscriberID),0)+1 INTO newID 
        FROM Subscribers;

        INSERT INTO Subscribers(SubscriberID,SubscriberName,SubscriptionDate)
        VALUES (newID,subName,CURDATE());
    ELSE
        SELECT CONCAT(subName,' already exists') AS Message;
    END IF;
END \\
DELIMITER ;
-- CALL AddSubscriberIfNotExists('suyash');