#ifndef ORO_OUTPUT_PORT_INTERFACE_HPP
#define ORO_OUTPUT_PORT_INTERFACE_HPP

#include "PortInterface.hpp"
#include "../internal/ListLockFree.hpp"
#include <boost/tuple/tuple.hpp>
#include "../os/Mutex.hpp"

namespace RTT
{ namespace base {

    /**
     * The base class of each OutputPort. It contains the connection management code, which is
     * independent of the actual data being transmitted.
     */
    class RTT_API OutputPortInterface : public PortInterface
    {
    protected:
        friend class internal::ConnFactory;

        typedef boost::tuple<PortID*, ChannelElementBase::shared_ptr, internal::ConnPolicy> ChannelDescriptor;
        internal::ListLockFree< ChannelDescriptor > connections;

        /** Helper method for disconnect(PortInterface*)
         *
         * This method removes the channel listed in \c descriptor from the list
         * of output channels if \c port has the same id that the one listed in
         * \c descriptor.
         *
         * @returns true if the descriptor matches, false otherwise
         */
        bool eraseIfMatchingPort(PortInterface const* port, ChannelDescriptor& descriptor);

        /** Helper method for disconnect()
         *
         * Unconditionally removes the given connection and return true
         */
        bool eraseConnection(OutputPortInterface::ChannelDescriptor& descriptor);

        /** Helper method for removeConnection(channel) */
        bool matchAndRemoveConnectionChannel(ChannelElementBase::shared_ptr channel, ChannelDescriptor const& descriptor) const;

        /** Helper method for port-to-port connection establishment.
         * This is the last step in adding a connection to an output port and
         * also validates if the connection is sound.
         * @return false if the connection failed to work, true otherwise.
         */
        bool addConnection(PortID* port_id, ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy);

        /** Helper method called by addConnection to set the channel's initial value, depending on the policy. */
        virtual bool connectionAdded( ChannelElementBase::shared_ptr channel_input, internal::ConnPolicy const& policy ) = 0;

        /** os::Mutex for when it is needed to resize the connections list */
        os::Mutex connection_resize_mtx;

    public:
        OutputPortInterface(std::string const& name);
        ~OutputPortInterface();

        /**
         * Returns true if this port records the last written value.
         */
        virtual bool keepsLastWrittenValue() const = 0;

        /**
         * Change the setting for keeping the last written value.
         * Setting this to false will clear up any unneeded storage.
         * If set, this port can initialize new connections with a data sample and
         * allows real-time data transport of dynamically sized objects
         * over its newly created connections.
         * @see OutputPort::OutputPort.
         */
        virtual void keepLastWrittenValue(bool new_flag) = 0;

        virtual void disconnect();

        /** Returns true if there is at least one channel registered in this
         * port's list of outputs
         */
        virtual bool connected() const;

        virtual void write(DataSourceBase::shared_ptr source);

        /** Connects this write port to the given read port, using a single-data
         * policy with the given locking mechanism
         */
        bool createDataConnection( InputPortInterface& sink, int lock_policy = internal::ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using a buffered
         * policy, with the buffer of the given size and the given locking
         * mechanism
         */
        bool createBufferConnection( InputPortInterface& sink, int size, int lock_policy = internal::ConnPolicy::LOCK_FREE );

        /** Connects this write port to the given read port, using the as policy
         * the default policy of the sink port
         */
        bool createConnection( InputPortInterface& sink );

        virtual bool createConnection( InputPortInterface& sink, internal::ConnPolicy const& policy ) = 0;

        /** Removes the channel that connects this port to \c port */
        void disconnect(PortInterface& port);

        /** Removes the connection associated with this channel, and the channel
         * as well
         */
        bool removeConnection(ChannelElementBase::shared_ptr channel);

        virtual bool connectTo(PortInterface& other, internal::ConnPolicy const& policy);

        virtual bool connectTo(PortInterface& other);
    };
}}

#endif
