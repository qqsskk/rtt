#ifndef ORO_INPUT_PORT_INTERFACE_HPP
#define ORO_INPUT_PORT_INTERFACE_HPP

#include <string>
#include "PortInterface.hpp"
#include "../Event.hpp"

namespace RTT
{ namespace base {


    /**
     * The base class of the InputPort. It contains the connection management code, which is
     * independent of the actual data being transmitted.
     */
    class RTT_API InputPortInterface : public PortInterface
    {
    public:
        typedef Event<void(PortInterface*)> NewDataOnPortEvent;

    protected:
        ChannelElementBase* channel;
        internal::ConnPolicy          default_policy;
        NewDataOnPortEvent* new_data_on_port_event;

    public:
        InputPortInterface(std::string const& name, internal::ConnPolicy const& default_policy = internal::ConnPolicy());
        ~InputPortInterface();

        /** Sets the channel from which this port should get samples
         *
         * You should usually not use this directly. Use createConnection
         * instead.
         */
        virtual void setInputChannel(ChannelElementBase* channel_output);

        /** Clears the input channel
         *
         * You should usually not use this directly. Use createConnection
         * instead.
         */
        virtual void clearInputChannel();

        internal::ConnPolicy getDefaultPolicy() const;

        /** Returns a DataSourceBase interface to read this port. The returned
         * data source is always the same object and will be destroyed when the
         * port is destroyed.
         */
        virtual DataSourceBase* getDataSource() = 0;

        /** Reads the port and updates the value hold by the given data source.
         * This is only valid for local ports.
         *
         * \a source has to be an assignable data source
         */
        virtual bool read(DataSourceBase::shared_ptr source);

        /** Removes any connection that either go to or come from this port */
        virtual void disconnect();

        /** Returns true if this port is connected */
        virtual bool connected() const;

        /** Clears the connection. After call to read() will return false after
         * clear() has been called
         */
        void clear();

        /**
         * Call this to indicate that the connection leading to this port
         * is ready to use. The input port will check its channel elements
         * by sending an inputReady() message. If this succeeds, this
         * function returns true and the input port is ready to use (this->connected() == true).
         * If sending inputReady() returns failure, this method returns
         * false and the connection is aborted (this->connected() == false).
         */
        virtual bool channelsReady();

        /** Returns the event object that gets emitted when new data is
         * available for this port. It gets deleted when the port is deleted.
         */
        NewDataOnPortEvent* getNewDataOnPortEvent();

        virtual bool connectTo(PortInterface& other, internal::ConnPolicy const& policy);

        virtual bool connectTo(PortInterface& other);
};

}}

#endif
