#ifndef TWISTERX_ALL_TO_ALL_H
#define TWISTERX_ALL_TO_ALL_H

#include<vector>
#include<unordered_map>
#include<queue>
#include<unordered_set>

#include "channel.hpp"

namespace twisterx {
class ReceiveCallback {
public:
  /**
   * This function is called when a data is received
   * @param source the source
   * @param buffer the buffer allocated by the system, we need to free this
   * @param length the length of the buffer
   * @return true if we accept this buffer
   */
  virtual bool onReceive(int source, void * buffer, int length) = 0;

  /**
   * Receive the header, this happens before we receive the actual data
   * @param source the source
   * @param buffer the header buffer, which can be 6 integers
   * @param length the length of the integer array
   * @return true if we accept the header
   */
  virtual bool onReceiveHeader(int source, int finished, int * buffer, int length) = 0;

  /**
   * This method is called after we successfully send a buffer
   * @param target
   * @param buffer
   * @param length
   * @return
   */
  virtual bool onSendComplete(int target, void * buffer, int length) = 0;
};

enum AllToAllSendStatus {
  ALL_TO_ALL_SENDING,
  ALL_TO_ALL_FINISH_SENT,
  ALL_TO_ALL_FINISHED
};

struct AllToAllSends {
  std::queue<std::shared_ptr<TxRequest>> requestQueue;
  std::queue<std::shared_ptr<TxRequest>> pendingQueue;
  int messageSizes{};
  AllToAllSendStatus sendStatus = ALL_TO_ALL_SENDING;
};

/**
 * The all to all communication. We insert values and wait until it completes
 */
class AllToAll : public ChannelReceiveCallback, ChannelSendCallback {
public:
  /**
   * Constructor
   * @param worker_id
   * @param all_workers
   * @return
   */
  AllToAll(int worker_id, const std::vector<int>& source, const std::vector<int>& targets, int edgeId,
      ReceiveCallback * callback);

  /**
   * Insert a buffer to be sent, if the buffer is accepted return true
   *
   * @param buffer the buffer to send
   * @param length the length of the message
   * @param target the target to send the message
   * @return true if the buffer is accepted
   */
  int insert(void *buffer, int length, int target, int * header, int headerLength);


  /**
   * Insert a buffer to be sent, if the buffer is accepted return true
   *
   * @param buffer the buffer to send
   * @param length the length of the message
   * @param target the target to send the message
   * @return true if the buffer is accepted
   */
  int insert(void *buffer, int length, int target);

  /**
   * Check weather the operation is complete, this method needs to be called until the operation is complete
   * @return true if the operation is complete
   */
  bool isComplete();

  /**
   * When this function is called, the operation finishes at both receivers and targets
   * @return
   */
  void finish();

  /**
   * We implement the receive complete callback from channel
   * @param receiveId
   * @param buffer
   * @param length
   */
  void receivedData(int receiveId, void *buffer, int length) override;

  /**
   * We implement the send callback from channel
   * @param request the original request, we can free it now
   */
  void sendComplete(std::shared_ptr<TxRequest> request) override;

  /**
   * Receive the header, this happens before a message is received
   * @param receiveId the receive id
   * @param finished weather this is the last header
   * @param header the header as an set of integers
   * @param headerLength length of the header
   */
  void receivedHeader(int receiveId, int finished, int * header, int headerLength) override;

  /**
   * Close the operation
   */
  void close();
private:
  void sendFinishComplete(std::shared_ptr<TxRequest> request) override;

private:
  int worker_id;                 // the worker id
  std::vector<int> sources;  // the list of all the workers
  std::vector<int> targets;  // the list of all the workers
  int edge;                  // the edge id we are going to use
  std::unordered_map<int, AllToAllSends *>  sends; // keep track of the sends
  std::unordered_set<int> finishedSources;  // keep track of  the finished sources
  std::unordered_set<int> finishedTargets;  // keep track of  the finished targets
  bool finishFlag = false;
  Channel * channel;             // the underlying channel
  ReceiveCallback * callback;    // after we receive a buffer we will call this function
  unsigned long thisNumTargets;            // number of targets in this process, 1 or 0
  int thisNumSources;            // number of sources in this process, 1 or 0
};
}

#endif